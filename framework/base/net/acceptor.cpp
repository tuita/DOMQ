#include <base/net/acceptor.h>
#include <base/net/eventmanager.h>
#include <base/net/socket.h>
#include <base/net/eventhandler.h>
#include <base/log/log.h>
#include <base/third/scope_guard.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <base/log/log.h>

namespace base
{

Acceptor::Acceptor()
{
}
//
//Acceptor::Acceptor(int fd)
//{
//	GetEventObject().SetObject(fd);
//}

Acceptor::Acceptor(const SockAddress & stSockAddr)
	: _addr(stSockAddr)
{
}

Acceptor::~Acceptor()
{
}

int Acceptor::Open()
{
	bool newSocket = false;
	if(!this->GetEventObject().IsValid()) 
    {
		Socket listenSocket;

		if(_addr.GetFamily() == AF_LOCAL || _addr.GetFamily() == AF_FILE) 
        {
			::remove(_addr.ToString().c_str());
		}

		if(listenSocket.Create(_addr.GetFamily(), SOCK_STREAM, 0) != 0) 
        {
			return -1;
		}

		if(listenSocket.SetBlocking(false) != 0) 
        {
			listenSocket.Close();
			return -1;
		}

		if(_addr.GetFamily() == AF_INET && listenSocket.SetDelay(false) != 0) 
        {
			listenSocket.Close();
			return -1;
		}

		if(listenSocket.Bind(_addr.Getsockaddr(), _addr.Getsockaddrlen()) != 0) 
        {
			listenSocket.Close();
			return -1;
		}

		if(listenSocket.Listen(1024) != 0) 
        {
			listenSocket.Close();
			return -1;
		}

		SetEventObject(listenSocket.GetFd());
		newSocket = true;
	}
	else 
    {
		Socket listensock(GetEventObject().GetObject());

		if(listensock.SetBlocking(false) != 0) 
        {
			return -1;
		}

		listensock.SetDelay(false);

		if( listensock.GetLocalAddr(_addr) != 0 ) 
        {
			return -1;
		}
	}

	if(GetEventManager()->RegisterHandler(ReadMask | PersistMask, this) != 0)
	{
		if(newSocket) 
        {
			::close(GetEventObject().GetObject());
			GetEventObject().SetInvalid();
		}
		return -1;
	}

	return 0;
}

int Acceptor::Close()
{
	return GetEventManager()->RemoveHandler(ReadMask | WriteMask, this);
}

void Acceptor::HandleClose()
{
	::close(GetEventObject().GetObject());

    if(_addr.GetFamily() == AF_LOCAL || _addr.GetFamily() == AF_FILE) 
    {
        ::remove(_addr.ToString().c_str());
    }

	GetEventObject().SetInvalid();
}

void Acceptor::HandleInput()
{
	union {
		sockaddr generic;
		sockaddr_in  in_4;
		sockaddr_in6 in_6;
		sockaddr_un unix_domain;
	} stAddr;

	socklen_t addrlen = sizeof(stAddr);

	int handle = ::accept(GetEventObject().GetObject(), (sockaddr*)&stAddr.generic, &addrlen);
	if(handle == -1) 
    {
        LOG_ERROR("accpet socket error");
		return;
	}
	ScopeGuard closeguard = MakeGuard(::close, handle);

	SockAddress stRemoteAddr;
    LOG_DEBUG("new client connect");
	stRemoteAddr.Setsockaddr((sockaddr*)&stAddr);

	if(OnAccept(handle, stRemoteAddr) != 0) {
		return;
	}

	EventHandler* handler;
	if( (handler = CreateHandler(stRemoteAddr)) == NULL ) {
		return;
	}

	handler->SetEventObject(EventObject(handle));
	handler->SetEventManager(GetEventManager());
	if(handler->Open() != 0) 
    {
		DestroyHandler(handler);
		return;
	}

	OnConnected(handler);
	closeguard.Dismiss();
}

int Acceptor::OnAccept(int, const SockAddress &)
{
	return 0;
}

void Acceptor::OnConnected(EventHandler *)
{
}

} 

