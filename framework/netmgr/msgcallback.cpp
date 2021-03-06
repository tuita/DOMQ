#include <netmgr/msgcallback.h>
#include <netmgr/eventhandler.h>
#include <netmgr/handlerregistry.h>
#include <base/net/eventmanager.h>
#include <base/log/log.h>
#include <base/util/util.h>
#include <errno.h>
#include <netmgr/message.h>
#include <sys/socket.h>
#include <string>
#include <cassert>

namespace netmgr
{

int ExtractMsgCallBack::Call(void* p)
{
    if (_queue && p)
    {
        _queue->Put((Message*)p);
        return 0;
    }
    return -1;
    
}

int QueuePushMsgCallBack::Call(void* /*p*/)
{
    return ::send(_fd, "0", 1, MSG_DONTWAIT);
}

int PushMsgCallBack::Call(void* /*p*/)
{
    assert(_registry);
    assert(_queue);
    Message* m;
    while (_queue->Get(m, 0) == 0)
    {
        if (!m)
        {
            LOG_ERROR("recv from null msg to send");
            continue;
        }
        
        LOG_DEBUG("push msg:[%s] content:[%s], to client:[%s]", m->GetTypeName().c_str(), m->ToString().c_str(), m->context.peer.addr.c_str());
        gettimeofday(&m->context.gettime2, NULL);
        std::string addr = m->context.peer.addr;
        NetMgrEventHandler* handler = dynamic_cast<NetMgrEventHandler*>(_registry->GetHandler(addr));
        if (!handler)
        {
            LOG_WARN("Handler[%s] not found, maybe it disconnected, total time: %d, put time: %d us, get time: %d, process time: %d, put time2: %d, get time2: %d",
                addr.c_str(),
                base::time_interval(m->context.gettime2, m->context.starttime),
                base::time_interval(m->context.puttime, m->context.starttime),
                base::time_interval(m->context.gettime, m->context.puttime),
                base::time_interval(m->context.processtime, m->context.gettime),
                base::time_interval(m->context.puttime2, m->context.processtime),
                base::time_interval(m->context.gettime2, m->context.puttime2));
        }
        else
        {
            char buffer[1024*1024];
            ICoder * coder = handler->GetNetworkOption().coder;
            assert(coder);
            int len = coder->Encode(buffer, sizeof(buffer)-1, m);
            if (len < 0)
            {
                LOG_ERROR("encode data error, msg:[%s], content:[%s]", m->GetTypeName().c_str(), m->ToString().c_str());
                return -1;
            }
            
            if (len > 0)
            {
                handler->Send(buffer,  len);;
                LOG_DEBUG("send data:[%s], dest:[%s]", base::Escape(buffer, len).c_str(), addr.c_str());
            }
            else
            {
                handler->Close();
            }
        }
        m->Dispose();
    }
    return 0;
}

}

