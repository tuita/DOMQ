#include "addmsg.h"
#include <pmsg/addmsg.pb.h>
#include <base/log/log.h>

int AddRequest::ProtocolBufEncode(char* buf, size_t len) const 
{
    Test::AddMsgRequest req;
    req.set_isequenceid(serialNo);
    req.set_param1(param1);
    req.set_param2(param2);
    return (req.SerializeToArray(buf,len)) ? (req.ByteSize()) : -1 ;
}

int AddRequest::ProtocolBufDecode(const char* buf, size_t len)
{
    Test::AddMsgRequest req;
    if(!req.ParseFromArray(buf, len)) 
    {
        LOG_ERROR("parse data error");
        return -1;
    }
    serialNo = req.isequenceid();
    param1 = req.param1();
    param2 = req.param2();
    return 0;
}

int AddResult::ProtocolBufEncode(char* buf, size_t len) const 
{
    Test::AddMsgResult res;
    res.set_isequenceid(serialNo);
    res.set_error(error);
    if (error!=0)
    {
        res.set_errmsg(errMsg);
    }
    else
    {
        res.set_sum(sum);
    }
    
    return (res.SerializeToArray(buf,len)) ? (res.ByteSize()) : -1 ;
}

int AddResult::ProtocolBufDecode(const char* buf, size_t len)
{
    Test::AddMsgResult res;
    if(!res.ParseFromArray(buf, len)) 
    {
        LOG_ERROR("parse data error");
        return -1;
    }
    serialNo = res.isequenceid();
    error = res.error();
    if (error!=0)
    {
        errMsg = res.errmsg();
    }
    else
    {
        sum = res.sum();
    }
    return 0;
}
