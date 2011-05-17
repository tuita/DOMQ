#ifndef _NETMGR_MESSAGE_H_
#define _NETMGR_MESSAGE_H_

#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <base/util/dispose.h>
#include <base/util/callback.h>

#define  _MSG_SERIALISE_PROPERTY(STRINGSTREAM, VAR) do { STRINGSTREAM << #VAR << ':' << VAR << Message::SEP; } while (false)

namespace Json
{
    class Value;
}

namespace netmgr
{

struct Pid
{
    unsigned int remote;
    unsigned int id;
    unsigned int time;
};

struct EndPoint
{
    size_t handleId;
    std::string addr ;
    Pid pid;
    size_t msgId;

    std::string ToString()
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer)-1, "handle:%zd, addr:%s, msgId:%zd",  handleId, addr.c_str(), msgId);
        return std::string(buffer);
    }

    void Swap(EndPoint& endpoint);
};

struct MsgContext
{
    timeval      starttime;
    timeval      puttime;
    timeval      gettime;
    timeval      processtime;
    timeval      puttime2;
    timeval      gettime2;
    
    EndPoint peer;

};


class Message: public base::IDispose
{
public:
    static const char SEP = ',';

    Message(base::CallBack* disposeCallBack = NULL):_disposeCallBack(disposeCallBack)
    {}

    virtual ~Message(){}

    std::string GetTypeName() const;

    virtual int ProtocolBufEncode(char* /*buf*/, size_t /*len*/) const
    {return 0;}

    virtual int ProtocolBufDecode(const char* /*buf*/, size_t /*len*/)
    {return 0;}

    virtual int JsonEncode(Json::Value& /*json*/) const
    {return 0;}
    virtual int JsonDecode(const Json::Value& /*json*/)
    {return  0;}

    std::string ToString() const;
    virtual void ToStr(std::stringstream& st) const;

    virtual void Dispose()
    {
        if(_disposeCallBack)
        {
            _disposeCallBack->Call(this);
        }
        else
        {
            delete this;
        }
    }
    
    int serialNo;
    /*EndPoint peer;*/
    MsgContext context;
protected:
    base::CallBack* _disposeCallBack;
};

class Request:public Message
{
    
};

class  Result:public Message
{
public:
    int error;
    std::string errMsg;
public:
    int getErrNo() { return this->error;}
    void setErrNo(int error) { this->error = error;}

    std::string getErrMsg() { return errMsg;}
    void setErrMsg(const std::string& msg) { errMsg = msg;}

    void ToStr(std::stringstream& st)const;
};

class LineMessage:public Message
{
public :
    std::string line;

    virtual int ProtocolBufEncode(char* buf, size_t len) const;
    virtual int ProtocolBufDecode(const char* buf, size_t len);

    void ToStr(std::stringstream& st)const;
};

class ServiceMessage:public Message
{
public :
    std::string service;
    std::string key;
    std::string msgContent;

    void ToStr(std::stringstream& st)const;

};
}

#endif



