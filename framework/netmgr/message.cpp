#include "message.h"
#include <typeinfo>
#include <string.h>
#include <base/log/log.h>
#include <cxxabi.h>
#include <base/util/util.h>


namespace netmgr{

//#define  _ACN_MESSGE_SWAP_PROPERTY(OBJ, PROPERTYNAME) do { std::swap(OBJ.PROPERTYNAME, PROPERTYNAME);} while (false)




void EndPoint::Swap(EndPoint& endpoint)
{
    std::swap(endpoint.addr, addr);
    std::swap(endpoint.handleId, handleId);
    std::swap(endpoint.pid, pid);
}

std::string Message::GetTypeName() const
{
    char buffer[128];
    size_t buflen = sizeof(buffer);
    int status;
    char* unmangled = abi::__cxa_demangle(typeid(*this).name(), buffer, &buflen, &status);
    if (status) return typeid(*this).name();
    return unmangled;
}



std::string Message::ToString() const
{
    std::stringstream st;
    ToStr(st);
    return st.str();
}

void Message::ToStr(std::stringstream& st)const 
{
    _MSG_SERIALISE_PROPERTY(st, serialNo);
    _MSG_SERIALISE_PROPERTY(st, context.peer.addr);
    _MSG_SERIALISE_PROPERTY(st, context.peer.msgId);
}

void Result::ToStr(std::stringstream& st)const
{
    Message::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, error);
    if (error !=0)
    {
        _MSG_SERIALISE_PROPERTY(st, errMsg);
    }
}


void LineMessage::ToStr(std::stringstream& st)const
{
    Message::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, line);
}

void ServiceMessage::ToStr(std::stringstream& st)const
{
    Message::ToStr(st);
    _MSG_SERIALISE_PROPERTY(st, service);
    _MSG_SERIALISE_PROPERTY(st, key);
    _MSG_SERIALISE_PROPERTY(st, base::Escape(msgContent));
}

}


    
