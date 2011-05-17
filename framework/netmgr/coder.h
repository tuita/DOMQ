#ifndef _NETMGR_CODER_H_
#define _NETMGR_CODER_H_
#include <map>
#include <list>
#include <ext/hash_map>
#include <base/net/sockaddr.h>
#include <netmgr/message.h>
#include <base/util/callback.h>


namespace base
{
    class IMutex;
}

namespace netmgr{

struct PortInfo
{
    base::SockAddress remoteAddr;
    PortInfo(const base::SockAddress& addr)
    {
        remoteAddr = addr;
    }
};

class ICoder
{
public:
    ICoder(base::CallBack* callback = NULL):_extractMsgCallback(callback) {}
    virtual ~ICoder(){}
    void SetHeadLen(size_t headLen){_headLen = headLen;}
    int  Extract(const char* ptr, size_t len, const base::SockAddress& addr);
	virtual Message* Decode(const char* buf, size_t len,  const PortInfo& portInfo) const=0 ;
	virtual int Encode(char* buf, size_t len,  const Message* message) const = 0;

protected:
    size_t _headLen;
    static const int MAXHEADLEN = 32;
    base::CallBack* _extractMsgCallback;
};

class ServiceFlagCoder: public ICoder
{
public:
    ServiceFlagCoder(base::IMutex* mutex):_coder(NULL), _internalCoder(NULL), _mutex(mutex){}

    virtual Message* Decode(const char* buf, size_t len,  const PortInfo& portInfo) const;
    virtual int Encode(char* buf, size_t len,  const Message* message) const;

    void SetCoder(ICoder* coder){ _coder = coder;}
    void SetInternalCoder(ICoder* coder) { _internalCoder = coder;}

protected:
    struct ServiceMsgInfo {
        std::string service;
        std::string key;
        int serialNo;
    };

    class RequestManager {
        int reqCounter;
        typedef __gnu_cxx::hash_map<int, ServiceMsgInfo> ID2MSG;
        ID2MSG _requests;

    public:
        int  RegisterRequest(const ServiceMsgInfo& svcMsgInfo);
        void UnregisterRequest(int reqID);
        int  GetRequest(int reqID, ServiceMsgInfo& svcMsgInfo);
    };
protected:
    ICoder* _coder;
    ICoder* _internalCoder;
    mutable RequestManager _requestManager;
    base::IMutex* _mutex;
};

class LineCoder:public ICoder
{
public :
    static LineCoder* Instance();
    virtual Message* Decode(const char* buf, size_t len,  const PortInfo& portInfo) const;
    virtual int Encode(char* buf, size_t len,  const Message* message) const; 
};

class JsonCoder: public ICoder
{
public :
    static JsonCoder* Instance();
    virtual Message* Decode(const char* buf, size_t len,  const PortInfo& portInfo) const ;
    virtual int Encode(char* buf, size_t len,  const Message* message) const; 
};

class JsonRpcCoder: public ICoder
{
public:
    virtual Message* Decode(const char* buf, size_t len, const PortInfo& portInfo) const;
    virtual int Encode(char* buf, size_t len, const Message* message) const;

    int RegisterName(const std::string& method, const std::string& req, const std::string& res);
protected:
    
    int DecodeRequest(const Json::Value& obj, Request& req) const;
    int EncodeResult(const Result& res, Json::Value& obj) const;

    std::string GetMethodName(const std::string& className) const;
    std::string GetReqClassName(const std::string& method) const;
    std::string GetResClassName(const std::string& method) const;

private:
    typedef std::map<std::string, std::pair<std::string, std::string> >  METHOD2CLASS;
    METHOD2CLASS _method2Class;

    typedef std::map<std::string, std::string> REQ2METHOD;
    REQ2METHOD _req2method;
    
    typedef std::map<std::string, std::string> RES2METHOD;
    RES2METHOD _res2method;

};

}

#endif

