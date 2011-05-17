#ifndef _BASE_ACCEPTOR_H_
#define _BASE_ACCEPTOR_H_

#include <base/net/eventhandler.h>
#include <base/net/sockaddr.h>

namespace base
{

class Acceptor : public EventHandler
{
public:

	Acceptor();

	Acceptor(const SockAddress & stSockAddr);

	virtual ~Acceptor();
	
	virtual int Open();

	virtual int Close();

	virtual void HandleClose();
	
	virtual void HandleInput();

	virtual void DestroyHandler(EventHandler* handler) = 0;

	inline void SetAddr(const SockAddress & stAddr) { _addr = stAddr; }

	inline const SockAddress & GetAddr() { return _addr; }

	virtual void HandleOutput(){};
	virtual void HandleTimeout(void * /*pTimerID*/){};

protected:

	virtual int OnAccept(int fd, const SockAddress & addr);

	virtual void OnConnected(EventHandler* eventHandler);

	virtual EventHandler* CreateHandler(const SockAddress & RemoteAddr) = 0;

protected:
	SockAddress _addr;
};

}

#endif

