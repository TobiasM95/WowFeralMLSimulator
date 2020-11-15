//
// Created by Matt on 10/10/2020.
//

#ifndef FERAL_SIM_COMMUNICATIONHANDLER_H
#define FERAL_SIM_COMMUNICATIONHANDLER_H

#define FERAL_SIM_GLOBAL_COMM_PIPE_ID "\\\\.\\pipe\\FERAL_SIM_GLOBAL_COMM_PIPE"
#define FERAL_SIM_INFINITE_PIPE_TIMEOUT NMPWAIT_WAIT_FOREVER
#define FERAL_SIM_DEFAULT_PIPE_TIMEOUT 3000
#define FERAL_SIM_PIPE_SIZE 4096
#define FERAL_SIM_NULL_PIPE_SWAP_ID "-1"

#include "windows.h"
#include "message.h"
#include <string>
#include <iostream>

class CommunicationHandler
{
public:
    CommunicationHandler();
    ~CommunicationHandler();
    Message getMessage(std::string& header, const int& num_attempts); //Wait for a specific message
    Message getMessage(); //Wait until the next message

    void connect(LPCSTR pipeName, const int& numAttempts);
    void connect(LPCSTR pipeName, const int& numAttempts, const int& pipeTimeout);
    void disconnect();
    void reconnect();

    bool sendMessage(const std::string& header, const std::string& body);
    bool sendMessage(Message* m);
    bool isConnected();

private:
    std::string currentPipeID;
    HANDLE pipe;
    bool connected;

    bool openPipe(LPCSTR pipeName, const int& timeout);
    bool openPipe(LPCSTR pipeName);
    void closePipe();

};


#endif //FERAL_SIM_COMMUNICATIONHANDLER_H
