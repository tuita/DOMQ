#ifndef _MSG_QUEUE_ERRNO_H__
#define _MSG_QUEUE_ERRNO_H__
struct MsgQueueErrorNo
{
    enum
    {
        SUCCESS = 0,
        NO_EXIST = 1001, 
        ALREADY_EXSIST = 1002, 
        QUEUECODE_NOEXIST = 1003,
        ALREADY_OPEN = 1004,
        MSG_LIMIT = 1005,
        MSGTYPE_ERROR = 1006,
        NO_SUB = 1007, 
        INTERNAL_ERROR = -1
    };
};

const char* MsgQueueError(int error);
#endif
