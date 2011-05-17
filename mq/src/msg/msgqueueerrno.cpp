#include "msgqueueerrno.h"
#include <hiredis/hiredis.h>

#define  _CASE_ERRNO_ERR(_ERRNO_) case _ERRNO_: return #_ERRNO_
const char* MsgQueueError(int error)
{
    switch (error)
    {
        _CASE_ERRNO_ERR(MsgQueueErrorNo::ALREADY_EXSIST);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::NO_EXIST);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::QUEUECODE_NOEXIST);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::ALREADY_OPEN);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::MSG_LIMIT);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::MSGTYPE_ERROR);
        _CASE_ERRNO_ERR(MsgQueueErrorNo::NO_SUB);
        _CASE_ERRNO_ERR(REDIS_ERR_IO);
        _CASE_ERRNO_ERR(REDIS_ERR_OTHER);
        _CASE_ERRNO_ERR(REDIS_ERR_EOF);
        _CASE_ERRNO_ERR(REDIS_ERR_PROTOCOL);
        
    default:
        return "UNKOWN";

    }
}