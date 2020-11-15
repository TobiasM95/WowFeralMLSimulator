//
// Created by Matt on 10/10/2020.
//


#include "communication_handler.h"
CommunicationHandler::CommunicationHandler() : currentPipeID(std::string()), connected(false), pipe(nullptr) {}

Message CommunicationHandler::getMessage(std::string& header, const int& num_attempts)
{
    for (int i = 0; i < num_attempts; i++)
    {
        Message msg = getMessage();
        //FERAL_SIM::Shared::cvarManager->log("Attempting to get message with header "+header+"...");
        if (msg.getHeader() == header)
        {
            return msg;
        }
    }

    return Message(FERAL_SIM_NULL_MESSAGE_HEADER, FERAL_SIM_NULL_MESSAGE_BODY);
}

Message CommunicationHandler::getMessage()
{
    Message msg(FERAL_SIM_NULL_MESSAGE_HEADER, FERAL_SIM_NULL_MESSAGE_BODY);
    if (!connected) { return msg; }

    DWORD read;
    char buffer[FERAL_SIM_PIPE_SIZE];
    ReadFile(pipe, buffer, FERAL_SIM_PIPE_SIZE, &read, nullptr);

    if (read == 0)
    {
        std::ostringstream os;
        os << GetLastError();
        std::cout << "ERROR: Receive message from server failed. Disconnecting pipe!\n" << os.str() << "\n";

        disconnect();

        return msg;
    }

    std::string messageString(buffer);
    msg = Message(messageString);

    return msg;
}

bool CommunicationHandler::sendMessage(const std::string& header, const std::string& body)
{
    Message msg(header, body);
    return sendMessage(&msg);
}

bool CommunicationHandler::sendMessage(Message* m)
{
    if (pipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR: Attempted to send a message with an invalid handle value!\n";
        return false;
    }
    std::string serialized = m->serialize();
    DWORD written;
    WriteFile(pipe, serialized.c_str(), serialized.length(), &written, nullptr);

    if (written < serialized.length())
    {
        std::ostringstream os;
        os << GetLastError();
        std::cout << "ERROR: Send message to server failed.\n" << "Wrote " + std::to_string(written) + " of " +
            std::to_string(serialized.length()) + "\n" << os.str() + "\n";

        return false;
    }
    return true;
}

void CommunicationHandler::connect(LPCSTR pipeName, const int& numAttempts)
{
    connect(pipeName, numAttempts, FERAL_SIM_DEFAULT_PIPE_TIMEOUT);
}

void CommunicationHandler::connect(LPCSTR pipeName, const int& numAttempts, const int& pipeTimeout)
{
    std::cout << "Attempting to connect to pipe " + std::string(pipeName) + "...\n";
    bool success = false;
    int attempt = 0;
    while (!success && attempt < numAttempts)
    {
        std::cout << "Attempt # " + std::to_string(attempt) + " to open pipe " + std::string(pipeName) + "\n";
        success = openPipe(pipeName, pipeTimeout);
        attempt++;
        Sleep(1000);
    }

    if (!success)
    {
        std::cout << "ERROR: UNABLE TO CONNECT TO PIPE NAMED " + std::string(pipeName) + " RESTART APPLICATION\n";
    }
    else
    {
        std::cout << "Successfully connected to pipe " + std::string(pipeName) + "\n";
    }
}

bool CommunicationHandler::openPipe(LPCSTR pipeName)
{
    return openPipe(pipeName, FERAL_SIM_DEFAULT_PIPE_TIMEOUT);
}

bool CommunicationHandler::openPipe(LPCSTR pipeName, const int& timeout)
{
    std::cout << "Checking current pipe status\n";
    if (isConnected()) { closePipe(); }

    connected = false;
    std::cout << "Waiting for pipe...\n";
    if (!WaitNamedPipe(pipeName, timeout))
    {
        std::cout << "ERROR: No pipe instances available.\n";
        closePipe();
        return false;
    }

    std::cout << "Found available pipe instance, connecting...\n";
    pipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR: Connection to pipe failed.\n";
        closePipe();
        return false;
    }

    std::cout << "Connected to pipe, setting mode...\n";
    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(pipe, &mode, nullptr, nullptr))
    {
        std::cout << "ERROR: Set pipe mode failed.\n";
        closePipe();
        return false;
    }

    currentPipeID = std::string(pipeName);
    std::cout << "Set pipe ID to " + std::string(pipeName) + "\nPipe successfully set up!";
    connected = true;
    return true;
}

void CommunicationHandler::reconnect()
{
    std::cout << "Attempting to reconnect to " + currentPipeID + "\n";
    disconnect();
    connect(currentPipeID.c_str(), 100);
}

void CommunicationHandler::disconnect()
{
    closePipe();
}

void CommunicationHandler::closePipe()
{
    connected = false;
    std::cout << "Closing current pipe! " + currentPipeID + "\n";
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);
}

bool CommunicationHandler::isConnected()
{
    return connected;
}

CommunicationHandler::~CommunicationHandler() {}