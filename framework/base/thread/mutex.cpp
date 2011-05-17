#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <base/thread/mutex.h>

namespace base
{

NilMutex * NilMutex::Instance()
{
    static NilMutex oNullMutex;
    return &oNullMutex;
}

}

