#include "msgqueuemsg.h"
#include "msgqueueerrno.h"
#include <stdint.h>
#include <cassert>
#include <sstream>
#include <base/util/jsonutil.h>
#include <json/json.h>
#include <base/log/log.h>

#define CHECK_JSON_OBJECT(JSONOBJ, OBJ_TYPE) \
    if (JSONOBJ.type() != OBJ_TYPE) { \
    LOG_ERROR("json type[%s] invalid, %d, needed, json obj:[%s]", #OBJ_TYPE, OBJ_TYPE, JSONOBJ.asCString()); \
    return -1; \
    } 

#define CHECK_JSON_KEY(JSONOBJ, KEY, VAR, OBJ_TYPE_VAR) \
    Json::Value VAR = JSONOBJ.get(KEY, Json::Value::null); \
    if (VAR.type() != OBJ_TYPE_VAR) { \
    LOG_ERROR("%s type invalid", KEY); \
    return -1; \
    }


#define GET_JSON_VALUE(JSOBJ, KEY, OBJ_TYPE_VAR, CONVERT_FUNC) \
    CHECK_JSON_KEY(JSOBJ, #KEY, KEY##Value, OBJ_TYPE_VAR) \
    KEY = KEY##Value.CONVERT_FUNC();

int OpenMsgQueueRequest::JsonDecode(const Json::Value& json)
{
    CHECK_JSON_OBJECT(json, Json::objectValue);
    GET_JSON_VALUE(json, msgQueueName, Json::stringValue, asString);
    GET_JSON_VALUE(json, role, Json::intValue, asInt);
    GET_JSON_VALUE(json, option, Json::intValue, asInt);
    GET_JSON_VALUE(json, duration, Json::intValue, asInt);
    GET_JSON_VALUE(json, create, Json::intValue, asInt);
    GET_JSON_VALUE(json, clientName, Json::stringValue, asString);
    return 0;
}

int OpenMsgQueueRequest::JsonEncode(Json::Value& /*json*/) const 
{
    assert("no ipleent" && false);
    return 0; 
}

int OpenMsgQueueResult::JsonDecode(const Json::Value& /*json*/)
{
    assert("OpenMsgQueueResult::JsonDecode no ipleent" && false);
    return 0;
}

int OpenMsgQueueResult::JsonEncode(Json::Value& json) const 
{
    if (error == 0)
    {
        json["msgQueueCode"] = ToJsonValue(msgQueueCode);
    }
    return 0; 
}

void OpenMsgQueueRequest::ToStr(std::stringstream& st) const
{
    Request::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, msgQueueName);
    _MSG_SERIALISE_PROPERTY(st, role);
    _MSG_SERIALISE_PROPERTY(st, option);
    _MSG_SERIALISE_PROPERTY(st, duration);
}

int CloseMsgQueueRequest::JsonDecode(const Json::Value& json)
{
    CHECK_JSON_OBJECT(json, Json::objectValue);
    GET_JSON_VALUE(json, msgQueueCode, Json::stringValue, asString);
    return 0;
}

int CloseMsgQueueRequest::JsonEncode(Json::Value& /*json*/) const 
{
    return 0; 
}

int CloseMsgQueueResult::JsonDecode(const Json::Value& /*json*/)
{
    return 0;
}

int CloseMsgQueueResult::JsonEncode(Json::Value& /*json*/) const 
{
    return 0; 
}

int SendMsgRequest::JsonDecode(const Json::Value& json)
{
    
    CHECK_JSON_OBJECT(json, Json::objectValue);
    GET_JSON_VALUE(json, msgData, Json::stringValue, asString);
    /*GET_JSON_VALUE(json, serialNo, Json::intValue, asInt);*/
    GET_JSON_VALUE(json, msgQueueCode, Json::stringValue, asString);
    return 0; 
}

int SendMsgRequest::JsonEncode(Json::Value& /*json*/) const 
{
    assert("SendMsgRequest::JsonEncode noipleent"&& false);
    return 0; 
}

void SendMsgRequest::ToStr(std::stringstream& st) const
{
    Request::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, msgQueueCode);
    _MSG_SERIALISE_PROPERTY(st, msgData);
}

int SendMsgResult::JsonDecode(const Json::Value& /*json*/)
{
    assert("SendMsgResult::JsonDecode noipleent"&& false);
    return 0; 
}

int SendMsgResult::JsonEncode(Json::Value& /*json*/) const 
{
    return 0; 
}

int GetMsgRequest::JsonDecode(const Json::Value& json)
{
    /*GET_JSON_VALUE(json, serialNo, Json::intValue, asInt);*/
    GET_JSON_VALUE(json, msgQueueCode, Json::stringValue, asString);
    GET_JSON_VALUE(json, autoReply, Json::intValue, asInt);
    return 0; 
}

int GetMsgRequest::JsonEncode(Json::Value& json) const 
{
    assert("GetMsgRequest::JsonEncode noipleent"&& false);
    return 0; 
}

void GetMsgRequest::ToStr(std::stringstream& st) const
{
    Request::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, msgQueueCode);
    _MSG_SERIALISE_PROPERTY(st, autoReply);
}

int GetMsgResult::JsonDecode(const Json::Value& /*json*/)
{
    assert("GetMsgResult::JsonDecode"&& false);
    return 0; 
}

int GetMsgResult::JsonEncode(Json::Value& json) const 
{
    if (error == MsgQueueErrorNo::SUCCESS)
    {
        json["data"] = ToJsonValue(data);
        json["msgNo"] = ToJsonValue(msgNo);
    }
    return 0; 
}

void GetMsgResult::ToStr(std::stringstream& st) const
{
    Result::ToStr(st);
    if (error==0)
    {
        _MSG_SERIALISE_PROPERTY(st, msgNo);
        _MSG_SERIALISE_PROPERTY(st, data);
    }
    
}

