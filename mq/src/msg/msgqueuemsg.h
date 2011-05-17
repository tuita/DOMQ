#ifndef _MSG_QUEUE_MSG_H__
#define _MSG_QUEUE_MSG_H__

#include <netmgr/message.h>

enum MSGQUEUETYPE
{
    PEER2PEER = 0,
    SUB2PUB = 1,
};

class OpenMsgQueueRequest: public netmgr::Request
{
public:
    std::string msgQueueName;
    int role;
    int create;
    int option;
    int duration;
    std::string clientName;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;

    void ToStr(std::stringstream& st) const;
};

class OpenMsgQueueResult: public netmgr::Result
{
public:
    std::string msgQueueCode;
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class GetMsgRequest: public netmgr::Request
{
public:
    std::string msgQueueCode;
    bool autoReply ;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;

    void ToStr(std::stringstream& st) const;
};

class GetMsgResult: public netmgr::Result
{
public:
    int msgNo;
    std::string data;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;

    void ToStr(std::stringstream& st) const;
};

class ReplyMsgRequest: public netmgr::Request
{
public:
    std::string msgQueueCode;
    int msgNo;
    
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class ReplyMsgResult: public netmgr::Result
{
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class SendMsgRequest: public netmgr::Request
{
public:
    std::string msgQueueCode;
    int time;
    std::string msgData;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
    
    void ToStr(std::stringstream& st) const;
};

class SendMsgResult: public netmgr::Result
{
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class CloseMsgQueueRequest: public netmgr::Request
{
public:
    std::string msgQueueCode;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class CloseMsgQueueResult: public netmgr::Result
{
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class RemoveMsgQueueRequest: public netmgr::Request
{
public:
    std::string msgQueueCode;

    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

class RemoveMsgQueueResult: public netmgr::Result
{
    int JsonDecode(const Json::Value& json);
    int JsonEncode(Json::Value& json) const;
};

#endif

