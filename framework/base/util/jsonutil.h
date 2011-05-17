#ifndef _JSON_UTIL_H__
#define _JSON_UTIL_H__

#include <json/json.h>

template<typename T>
inline Json::Value ToJsonValue(const T& v)
{
    return Json::Value(v);
}

template<>
inline Json::Value ToJsonValue<uint64_t>(const uint64_t& v)
{
    return Json::Value((Json::UInt)v);
}

#endif

