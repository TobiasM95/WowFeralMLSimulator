//
// Created by Matt on 10/10/2020.
//

#include "message.h"

Message::Message()
{
    header = "";
    body = "";
}
Message::Message(std::string initialHeader, std::string initialBody)
{
    header = std::move(initialHeader);
    body = std::move(initialBody);
}
Message::Message(const std::string& serializedMessage)
{
    deserialize(serializedMessage);
}
std::string Message::serialize()
{
    return header + FERAL_SIM_HEADER_END_TOKEN + body + FERAL_SIM_BODY_END_TOKEN;
}

void Message::deserialize(const std::string& serializedMessage)
{
    size_t headerBeginPosition = serializedMessage.find(FERAL_SIM_HEADER_END_TOKEN);
    size_t bodyBeginPosition = serializedMessage.find(FERAL_SIM_BODY_END_TOKEN);
    size_t bodyOffset = headerBeginPosition + strlen(FERAL_SIM_HEADER_END_TOKEN);

    header = serializedMessage.substr(0, headerBeginPosition);
    body = serializedMessage.substr(bodyOffset, bodyBeginPosition - bodyOffset);
}

void Message::append(int data)
{
    body += FERAL_SIM_MESSAGE_DATA_DELIMITER + std::to_string(data);
}
void Message::append(float data)
{
    body += FERAL_SIM_MESSAGE_DATA_DELIMITER + std::to_string(data);
}
void Message::append(double data)
{
    body += FERAL_SIM_MESSAGE_DATA_DELIMITER + std::to_string(data);
}
void Message::append(const std::string& data)
{
    body += FERAL_SIM_MESSAGE_DATA_DELIMITER + data;
}
void Message::append(Message* other)
{
    body += FERAL_SIM_MESSAGE_DATA_DELIMITER + other->getBody();
}

Message::~Message() = default;