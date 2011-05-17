/* vim: set foldmethod=syntax foldlevel=6 foldenable: */

#include "factory.h"
#include <base/log/log.h>
#include <stdio.h>
#include <base/thread/mutex.h>
#include "message.h"

namespace netmgr {

MappedMsgFactory* GetGlobalMsgFactory()
{
    static MappedMsgFactory g_MsgFactory;
    return &g_MsgFactory;
}

int MappedMsgFactory::RegisterFactory(MsgFactory* factory)
{
    std::list<std::string> types = factory->KnownTypes();

    for (std::list<std::string>::const_iterator it = types.begin();
            it != types.end(); it ++)
    {
        LOG_DEBUG("register factory: %s -> %p", it->c_str(), factory);

        std::map<std::string, MsgFactory*>::const_iterator fit = factories.find(*it);
        if (fit != factories.end()) {
            if (fit->second != factory) {
                LOG_WARN("message factory already exists: %s", it->c_str());
                return -1;
            }
            LOG_DEBUG("re-register factory: %s", it->c_str());
        }
    }

    for (std::list<std::string>::const_iterator it = types.begin();
            it != types.end(); it ++)
    {
        factories.insert(std::make_pair(*it, factory));
    }

    return 0;
}

void MappedMsgFactory::UnregisterFactory(MsgFactory* factory)
{
    /*base::LockGuard lock(&_mutex);
    lock.Lock();*/
    std::list<std::string> types = factory->KnownTypes();

    for (std::list<std::string>::const_iterator it = types.begin();
            it != types.end(); it ++)
    {
        std::map<std::string, MsgFactory*>::iterator itfind = factories.find(*it);
        if (itfind != factories.end() && itfind->second == factory) {
            factories.erase(itfind);
        }
    }
}

std::list<std::string> MappedMsgFactory::KnownTypes() const
{
    /*base::LockGuard lock(&_mutex);
    lock.Lock();*/
    std::list<std::string> result;

    for (std::map<std::string, MsgFactory*>::const_iterator it = factories.begin();
            it != factories.end(); it ++)
    {
        result.push_back(it->first);
    }
    return result;
}

Message* MappedMsgFactory::Create(const char* type)
{
    std::map<std::string, MsgFactory*>::iterator it = factories.find(type);
    if (it == factories.end()) {
        LOG_WARN("create message fail: %s", type);
        return NULL;
    }
    LOG_DEBUG("create msg:[%s]", type);
    Message* msg = it->second->Create(type);
    return msg;
}

void MappedMsgFactory::Destroy(const Message* msg)
{
    if (!msg) return;

    std::map<std::string, MsgFactory*>::iterator it = factories.find(msg->GetTypeName());
    if (it == factories.end()) {
        LOG_WARN("destroy message fail: %s", msg->GetTypeName().c_str());
        return;
    }
    return it->second->Destroy(msg);
}

} 

