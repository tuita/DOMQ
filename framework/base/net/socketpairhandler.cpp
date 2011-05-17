#include <base/net/socketpairhandler.h>
#include <base/net/eventobject.h>
#include <base/net/eventmanager.h>
#include <base/util/callback.h>
#include <base/log/log.h>
#include <errno.h>
#include <cassert>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


namespace base{

int SocketPairHandler::Open()
{
	int fd[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) != 0)
	{
		LOG_ERROR("socket pair failed: %s", strerror(errno));
		return -1;
	}
    assert(GetEventManager());
    GetEventObject().SetObject(fd[0]);
	_anotherhandle = fd[1];

	int ret = GetEventManager()->RegisterHandler(ReadMask | PersistMask, this);
	if (ret != 0) 
    {
        LOG_ERROR("register handler error, ret:%d, error:%s", ret, strerror(errno));
        CloseHandles();
    }
    
	return ret;
}

void SocketPairHandler::HandleInput()
{
	char buffer[64];
	int ret = ::recv(int(), buffer, sizeof(buffer), 0);
	if (ret == -1 && errno == EINTR)
    {
        LOG_DEBUG("EINTR when recv");
		return;
    }

	if (ret <= 0)
	{
		LOG_ERROR("read notify socket fail: %s, remove all events", strerror(errno));
		GetEventManager()->RemoveHandler(ReadMask, this);
		return;
	}

	if(_callback)
    {
        std::pair<char* , size_t> data = std::make_pair(buffer, size_t(ret));
         _callback->Call(&data);
    }
}

void SocketPairHandler::HandleClose()
{
	LOG_DEBUG("SocketPairHandler::HandleClose()");
	CloseHandles();
}

void SocketPairHandler::CloseHandles()
{
	::close(int());
	::close(GetAnotherHandle());
	GetEventObject().SetInvalid();
	_anotherhandle = -1;
}

} // namespace base
