#ifndef ACN_EVENTHANDLER_H_
#define ACN_EVENTHANDLER_H_

#include <netmgr/networkoption.h>
#include <base/net/eventhandler.h>
#include <base/util/objectqueue.h>
#include <base/util/memblock.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <vector>

namespace netmgr
{
class NetMgrEventHandler : public base::EventHandler
{
public:
    NetMgrEventHandler(const NetworkOption* option = NULL);

	virtual ~NetMgrEventHandler();

    void SetPeerAddr(const base::SockAddress& addr)
    { this->_peerAddr = addr; }

    const base::SockAddress& GetPeerAddr() const
    { return this->_peerAddr; }

    void SetNetworkOption(const NetworkOption* option)
    {
        _networkOption = option;
    }

    const NetworkOption& GetNetworkOption() const
    {
        return *_networkOption;
    }

	virtual int Open();

	virtual int Close();

	virtual void HandleClose();

	virtual void HandleInput();

	virtual void HandleOutput();

	virtual void HandleTimeout(void *);

	void Send(const char* data, size_t len);

protected:
    const NetworkOption* _networkOption;
    base::SockAddress    _peerAddr;
    base::MemBlock   _recvbuffer;
    base::MemBlock   _sendbuffer;
};
} // namespace basen

#endif // MYEVENTHANDLER_H_

