#include "coder.h"
#include <base/thread/mutex.h>
#include <base/thread/mutexguard.h>
#include <json/json.h>
#include <netmgr/message.h>
#include <netmgr/factory.h>
#include <base/third/scope_guard.h>
#include <base/util/util.h>
#include <base/util/jsonutil.h>
#include <base/log/log.h>
#include <string.h>
#include <map>
#include <assert.h>
#include <arpa/inet.h>

namespace netmgr{

#define  _TRANSLATOR_BUF_SIZE 1024*1024*5 



int ICoder::Extract(const char* ptr, size_t len, const base::SockAddress& addr)
{
    size_t used = 0;
    while(true) 
    {
        if (len <= _headLen)
        {
            return used;
        }

        char buf[MAXHEADLEN];
        memcpy(buf, ptr, _headLen);
        buf[_headLen] = '\0';

        uint32_t bodyLen = ntohl(*(uint32_t*)buf);

        if (len < size_t(bodyLen))
        {
            LOG_DEBUG("extractor used:[%zd], data:[%s], len:[%zd], bodyLen:[%zd]", used, base::Escape(ptr, len).c_str(), len, bodyLen);
            return used;
        }
        Message* m = Decode(ptr+_headLen, bodyLen-_headLen, addr);
        if(!m)
        {
            LOG_WARN("create package fail, no enough memory or allocator limit");
            return -1;
        }
        m->context.peer.addr = addr.ToString();
        if (_extractMsgCallback)
        {
            _extractMsgCallback->Call(m);
        }

        ptr  += bodyLen;
        len  -= bodyLen;
        used += bodyLen;
    }
}


int ServiceFlagCoder::RequestManager::RegisterRequest(const ServiceFlagCoder::ServiceMsgInfo& svcMsgInfo)
{
    ++reqCounter;
    if( _requests.end() != _requests.find(reqCounter))
    {
        LOG_ERROR("register request error, sn:[%u]", reqCounter);
        return -1;
    }
    _requests[reqCounter] = svcMsgInfo;
    return reqCounter;
}

void ServiceFlagCoder::RequestManager::UnregisterRequest(int reqID)
{
    _requests.erase(reqID);
}

int ServiceFlagCoder::RequestManager::GetRequest(int reqID, ServiceFlagCoder::ServiceMsgInfo& svcMsgInfo)
{
    ID2MSG::iterator it = _requests.find(reqID);
    if(_requests.end() == it)
    {
        return -1;
    }
    svcMsgInfo = it->second;
    return 0;
}

Message* ServiceFlagCoder::Decode(const char* buf, size_t len, const PortInfo& portInfo) const
{
    assert(_coder);
    assert(_internalCoder);
    Message* msg = _coder->Decode(buf, len, portInfo);
    if (!msg)
    {
        LOG_ERROR("ServiceFlagCoder decode error, recv data:[%s], client:[%s]", 
            base::Escape(buf, len).c_str(), portInfo.remoteAddr.ToString().c_str());
        return NULL;
    }
    ServiceMessage* sMsg = dynamic_cast<netmgr::ServiceMessage*>(msg);
    if (!sMsg)
    {
        base::MutexGuard lock(_mutex);
        (void)lock;
        LOG_DEBUG("serviceFlagCoder recv msg:[%s], not servicemessage", msg->GetTypeName().c_str());
        ServiceMsgInfo info;
        info.serialNo = msg->serialNo;
        int reqNo = _requestManager.RegisterRequest(info);
        msg->serialNo = reqNo;
        return msg;
    }
    BASE_BLOCK_GUARD(&MappedMsgFactory::Destroy, *(GetGlobalMsgFactory()), msg);
    Message* req = _internalCoder->Decode(sMsg->msgContent.c_str(), sMsg->msgContent.size(), portInfo);
    if (!req)
    {
        LOG_ERROR("serviceFlagCoder creat msg error ,data:[%s]",  base::Escape(buf, len).c_str());
        return NULL;
    }

    req->context.peer = msg->context.peer;
    ServiceMsgInfo info;
    info.key = sMsg->key;
    info.service = sMsg->service;
    info.serialNo = sMsg->serialNo;
    base::MutexGuard lock(_mutex);
    (void)lock;
    int reqNo = _requestManager.RegisterRequest(info);
    req->serialNo = reqNo;
    return req;
}

int ServiceFlagCoder::Encode(char* buf, size_t len, const Message* message) const
{
    assert(_internalCoder);
    assert(_coder);
    ServiceMsgInfo info;
    base::MutexGuard lock(_mutex);
    (void)lock;
    if(0 != _requestManager.GetRequest(message->serialNo, info))
    {
        LOG_DEBUG("get msg no in request manager, seq:[%d]", message->serialNo);
        return _coder->Encode(buf, len, message);
    }
    BASE_BLOCK_GUARD(&ServiceFlagCoder::RequestManager::UnregisterRequest, _requestManager, message->serialNo);

    if (info.service=="" && info.key=="")
    {
        return _coder->Encode(buf, len, message);
    }
    int ret = _internalCoder->Encode(buf, len, message);
    if(ret<0)
    {
        LOG_ERROR("protocol encode error, msg:[%s]", message->GetTypeName().c_str());
        return -1;
    }
    ServiceMessage res;
    res.context.peer = message->context.peer;
    res.serialNo = info.serialNo;
    res.service = info.service;
    res.key = info.key;
    res.msgContent.assign(buf, ret);
    return _coder->Encode(buf, len, &res);
}


LineCoder* LineCoder::Instance()
{
    static LineCoder translator;
    return &translator;
}

int LineCoder::Extract(const char* ptr, size_t len, const base::SockAddress& addr)
{
    size_t used = 0 ;
    for (size_t i=0; i<len; ++i)
    {
        if(ptr[i]=='\n') 
        {
            Message* m = Decode(ptr+used, i-used+1, addr);
            used = i+1;
            if(!m)
            {
                LOG_WARN("create package fail, no enough memory or allocator limit");
                return used;
            }
            m->context.peer.addr = addr.ToString();
            if (_extractMsgCallback) _extractMsgCallback->Call(m);
        }
    }
    return used;
    
}
Message* LineCoder::Decode(const char* buf, size_t len, const PortInfo& /*portInfo*/) const
{
    LineMessage* msg = dynamic_cast<LineMessage*>(GetGlobalMsgFactory()->Create("netmgr::LineMessage"));
    if(msg)
    {
        msg->line.assign(buf, len);
    }
    return msg;
}

int LineCoder::Encode(char* buf, size_t len, const Message* message) const
{
    const LineMessage* msg = dynamic_cast<const LineMessage*>(message);
    if (!msg)
    {
        LOG_ERROR("LineCoder encode msg:[%s]", message->GetTypeName().c_str());
        return -1;
    }
    memcpy(buf, msg->line.c_str(), msg->line.size());
    return msg->line.size();
}

JsonCoder* JsonCoder::Instance()
{
    static JsonCoder translator;
    return &translator;
}

Message* JsonCoder::Decode(const char* buf, size_t len, const PortInfo& /*portInfo*/) const
{
    Json::Reader reader;
    Json::Value  obj;
    if (!reader.parse(std::string(buf, len), obj))
    {
        LOG_ERROR("parse json error, content:[%s]", base::Escape(buf, len).c_str());
        return NULL;
    }

    const char* className = obj["_msg_"].asCString();
    if (!className)
    {
        LOG_ERROR("get classname error, content:[%s]", base::Escape(buf, len).c_str());
        return NULL;
    }

    Message* msg = GetGlobalMsgFactory()->Create(className);
    if (!msg)
    {
        LOG_ERROR("create class error, class:[%s], content:[%s]", className, base::Escape(buf, len).c_str());
        return NULL;
    }
    LOG_DEBUG("msgname:[%s], content:[%s]", msg->GetTypeName().c_str(), msg->ToString().c_str());
    if(0 != msg->JsonDecode(obj))
    {
        LOG_DEBUG("msg:[%s] jsondecode error, content:[%s]", className, base::Escape(buf, len).c_str());
        return NULL;
    }
    
    return msg;
}

int JsonCoder::Encode(char* buf, size_t len, const Message* message) const
{
    Json::Value obj;
    if(0 != message->JsonEncode(obj))
    {
        LOG_ERROR("message:[%s] jsonencode error", message->ToString().c_str());
        return -1;
    }
    obj["_msg_"] = Json::Value(message->GetTypeName().c_str());
    const std::string& data = obj.asString();
    if (data.size() > len)
    {
        LOG_ERROR("data len:%zd, buf len:%zd", data.size(), len);
        return -1;
    }
    int size = htonl(data.size()+4);
    memcpy(buf, (const char*)&size, 4);
    memcpy(buf+4, data.c_str(), data.size());
    return 4+data.size();
}

int JsonRpcCoder::RegisterName(const std::string& method, const std::string& req, const std::string& res)
{
    if (_method2Class.find(method) == _method2Class.end())
    {
        LOG_ERROR("method:[%s] is exist", method.c_str());
        return -1;
    }

    if (_req2method.find(req) == _req2method.end())
    {
        LOG_ERROR("request:[%s] is exist", req.c_str());
        return -1;
    }

    if (_res2method.find(res) == _res2method.end())
    {
        LOG_ERROR("result:[%s] is exist", res.c_str());
        return -1;
    }

    _method2Class[method] = std::make_pair(req, res);
    _req2method[req] = method;
    _res2method[res] = method;
    return 0;
}

std::string JsonRpcCoder::GetMethodName(const std::string& className) const
{
    REQ2METHOD::const_iterator reqIt = _req2method.find(className);
    if (_req2method.end() != reqIt)
    {
        return reqIt->second;
    }

    RES2METHOD::const_iterator resIt = _res2method.find(className);
    if (_res2method.end() != resIt)
    {
        return resIt->second;
    }

    size_t pos = className.rfind("Request");
    if (pos != std::string::npos)
    {
        return className.substr(0, pos);
    }

    pos = className.rfind("Result");
    if (pos != std::string::npos)
    {
        return className.substr(0, pos);
    }

    return className;
}

std::string JsonRpcCoder::GetReqClassName(const std::string& method) const
{
    METHOD2CLASS::const_iterator it = _method2Class.find(method);
    if (_method2Class.end() != it)
    {
        return it->second.first;
    }
    return method+"Request";
}

std::string JsonRpcCoder::GetResClassName(const std::string& method) const
{
    METHOD2CLASS::const_iterator it = _method2Class.find(method);
    if (_method2Class.end() != it)
    {
        return it->second.second;
    }
    return method+"Result";
}

int JsonRpcCoder::EncodeResult(const Result& res, Json::Value& obj) const
{
    Json::Value msgObj;
    if(0 != res.JsonEncode(msgObj))
    {
        LOG_ERROR("serialNo:[%d], jsonencode error, res:[%s]", res.serialNo, res.ToString().c_str());
        return -1;
    }
    
    obj["id"] = ToJsonValue(Json::Int(res.context.peer.msgId));
    obj["errno"] = ToJsonValue(Json::Int(res.error));
    if (res.errMsg.size()>0)
    {
        obj["errmsg"] = ToJsonValue(res.errMsg);
    }
    if( msgObj.type() != Json::nullValue)
    {
        obj["result"] = msgObj;
    }
    std::string msgName = res.GetTypeName();
    const std::string& method = GetMethodName(msgName);
    obj["method"] = ToJsonValue(method);
    return 0;
}


int JsonRpcCoder::Encode(char* buf, size_t len, const Message* message) const
{
    Json::Value obj;
    
    const Result* res = dynamic_cast<const Result*>(message);
    if (!res)
    {
        LOG_ERROR("not implement msg:[%s] content:[%s] jsonRpc encode", message->GetTypeName().c_str(), message->ToString().c_str());
        return -1;
    }

    if(0 != EncodeResult(*res, obj))
    {
        return -1;
    }

    const std::string& data = obj.asString();
    if (data.size() > len)
    {
        LOG_ERROR("data len:%zd, buf len:%zd", data.size(), len);
        return -1;
    }
    int size = htonl(data.size()+_headLen);
    memcpy(buf, (const char*)&size, _headLen);
    memcpy(buf+_headLen, data.c_str(), data.size());
    return _headLen+data.size();
}

int JsonRpcCoder::DecodeRequest(const Json::Value& obj, Request& req) const
{
    if (0 != req.JsonDecode(obj["params"]))
    {
        return -1;
    }
    req.context.peer.msgId = obj["id"].asInt();
    return 0;
}

Message* JsonRpcCoder::Decode(const char* buf, size_t len, const PortInfo& portInfo) const
{
    Json::Reader reader;
    Json::Value  obj;
    LOG_DEBUG("decode data:[%s]", base::Escape(buf, len).c_str());
    if (!reader.parse(std::string(buf, len), obj))
    {
        LOG_ERROR("parse json error, content:[%s]", base::Escape(buf, len).c_str());
        return NULL;
    }
    LOG_DEBUG("parse data:[%s] succ", base::Escape(buf, len).c_str());

    const char* method = obj["method"].asCString();
    //todo: decode Result
    //
    //
    std::string className = GetReqClassName(method);
    Message* msg = GetGlobalMsgFactory()->Create(className.c_str());
    if (!msg)
    {
        LOG_ERROR("create class error, class:[%s], content:[%s]", className.c_str(), base::Escape(buf, len).c_str());
        return NULL;
    }
    LOG_DEBUG("create msg:[%s] succ", className.c_str());
    base::ScopeGuard guard = base::MakeObjGuard(*GetGlobalMsgFactory(), &MappedMsgFactory::Destroy, msg);
    Request* req = dynamic_cast<netmgr::Request*>(msg);
    if (!req)
    {
        LOG_ERROR("not implement msg:[%s] jsonRpc encode", className.c_str());
    }

    if(0 != DecodeRequest(obj, *req))
    {
        return NULL;
    }
    guard.Dismiss();
    return msg;
}

}

