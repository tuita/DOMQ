#include <list>
#include <string.h>
#include <netmgr/factory.h>
#include "msgfactory.h"
#include "addmsg.h"

std::list<std::string> MsgFactory::KnownTypes() const
{
    std::list<std::string> result;
    result.push_back("AddRequest");
    result.push_back("AddResult");
    return result;

}

#define NAME2OBJ(x) \
if (strcmp(type, #x) == 0) { \
return new x; \
}
netmgr::Message* MsgFactory::Create(const char* type)
{
    NAME2OBJ(AddRequest)
    NAME2OBJ(AddResult)

   return NULL;
}
#undef NAME2OBJ

void MsgFactory::Destroy(const netmgr::Message* msg)
{
    delete msg;
}

