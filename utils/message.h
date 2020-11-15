//
// Created by Matt on 10/10/2020.
//

#ifndef FERAL_SIM_MESSAGE_H
#define FERAL_SIM_MESSAGE_H
#define FERAL_SIM_MESSAGE_DATA_DELIMITER " "
#define FERAL_SIM_HEADER_END_TOKEN "FRSEHEADER"
#define FERAL_SIM_BODY_END_TOKEN "FRSEBODY"

#define FERAL_SIM_NULL_MESSAGE_HEADER "FRSNMH"
#define FERAL_SIM_NULL_MESSAGE_BODY "FRSNMB"
#define FERAL_SIM_CONFIG_MESSAGE_HEADER "FRSC"

#define FERAL_SIM_STATE_MESSAGE_HEADER "FRSSMH"
#define FERAL_SIM_AGENT_ACTION_MESSAGE_HEADER "FRSAAMH"
#define FERAL_SIM_RESET_GAME_STATE_MESSAGE_HEADER "FRSRGSMH"
#define FERAL_SIM_AGENT_ACTION_IMMEDIATE_RESPONSE_MESSAGE_HEADER "FRSAAIRMH"
#define FERAL_SIM_REQUEST_LAST_BOT_INPUT_MESSAGE_HEADER "FRSRLBIMH"

#include <string>
#include <sstream>
#include <utility>

class Message
{
public:
    Message();
    Message(std::string initialHeader, std::string initialBody);
    Message(const std::string& serializedMessage);
    ~Message();

    std::string serialize();
    void deserialize(const std::string& serializedMessage);

    void setHeader(std::string newHeader) { header = std::move(newHeader); }
    void setBody(std::string newBody) { body = std::move(newBody); }

    std::string getHeader() { return header; }
    std::string getBody() { return body; }

    void append(int data);
    void append(float data);
    void append(double data);
    void append(const std::string& data);
    void append(Message* other);
private:
    std::string header;
    std::string body;
};


#endif //FERAL_SIM_MESSAGE_H
