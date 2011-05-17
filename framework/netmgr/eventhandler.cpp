#include <netmgr/eventhandler.h>
#include <base/net/eventmanager.h>
#include <base/net/socket.h>
#include <base/log/log.h>
#include <base/util/objectqueue.h>
#include <stdio.h>
#include <cassert>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <base/util/util.h>

namespace netmgr
{

NetMgrEventHandler::NetMgrEventHandler(const NetworkOption* option):_networkOption(option)
{
	
}

NetMgrEventHandler::~NetMgrEventHandler()
{
}

int time_interval(const timeval& t1, const timeval& t2);

int NetMgrEventHandler::Open()
{
	
    LOG_DEBUG("open event handlers, client:[%s]", _peerAddr.ToString().c_str());
	_sendbuffer.Clear();
	_recvbuffer.Clear();
    
	timeval tv = {_networkOption->timeout/1000, _networkOption->timeout%1000*1000};
    return GetEventManager()->RegisterHandler(base::ReadMask, this, &tv);
}

int NetMgrEventHandler::Close()
{
    return GetEventManager()->RemoveHandler(base::ReadMask | base::WriteMask, this);
}

void NetMgrEventHandler::HandleInput()
{
	const int RECV_SIZE = 16384;
	size_t pos = _recvbuffer.Size();
	_recvbuffer.Resize(pos + RECV_SIZE);
	
	int ret = ::recv(GetEventObject().GetObject(), _recvbuffer.GetBegin() + pos, RECV_SIZE, 0);
	if (ret == 0)
	{
		LOG_DEBUG("Receive failed: client[%s] disconnectd", _peerAddr.ToString().c_str());
		Close();
	}
	else if (ret == -1)
	{
		if (errno == EINTR)
        {
			return;
        }
		else
		{
            LOG_WARN("Receive failed: %s, client:[%s]", strerror(errno), _peerAddr.ToString().c_str());
			Close();
		}
	}
	else
	{
        LOG_DEBUG("netinfo, recv data:[%s] from src:[%s]", base::Escape(_recvbuffer.GetBegin(), ret).c_str(), _peerAddr.ToString().c_str());
		_recvbuffer.AddSpace(ret);
        ICoder* coder = _networkOption->coder;
        assert(coder);
		int usecount = coder->Extract(_recvbuffer.GetBegin(), _recvbuffer.Size(), _peerAddr);
		if (usecount < 0) 
        {
            LOG_WARN("Extract package fail,data:[%s], client:[%s]", base::Escape(_recvbuffer.GetBegin(), _recvbuffer.Size()).c_str(), _peerAddr.ToString().c_str());
			Close();
		}
		else
		{
			if (usecount > 0)
			{
				_recvbuffer.Copy(_recvbuffer.GetBegin(), _recvbuffer.GetBegin() + usecount, _recvbuffer.Size() - usecount);
				_recvbuffer.ReduceSpace(usecount);
			}
			timeval tv = {_networkOption->timeout/1000, _networkOption->timeout%1000*1000};
            if (GetEventManager()->RegisterHandler(base::ReadMask, this, &tv) == -1)
			{
                LOG_ERROR("register read event fail, client:[%s]", _peerAddr.ToString().c_str());
				Close();
			}
		}
	}
}

void NetMgrEventHandler::HandleOutput()
{
	if (_sendbuffer.Size() <= 0)
    {
		return;
    }

	int ret = ::send(GetEventObject().GetObject(), _sendbuffer.GetBegin(), _sendbuffer.Size(), MSG_DONTWAIT);
	if (ret == 0)
	{
        LOG_ERROR("Send failed: client[%s] disconnectd", _peerAddr.ToString().c_str());
		Close();
	}
	else if (ret == -1)
	{
		if (errno == EINTR)
		{
			return;
		}
		else if (errno == EAGAIN)
		{
			timeval tv = {_networkOption->timeout, 0};
            if (GetEventManager()->RegisterHandler(base::WriteMask, this, &tv) == -1)
			{
                LOG_ERROR("register write event fail, client:[%s]", _peerAddr.ToString().c_str());
				Close();
			}
		}
		else
		{
            LOG_WARN("Receive failed: %s, client:[%s]", strerror(errno), _peerAddr.ToString().c_str());
			Close();
		}
	}
	else
	{
        LOG_DEBUG("netinfo, send data:[%s] to dest:[%s] succ", base::Escape(_sendbuffer.GetBegin(), ret).c_str(), _peerAddr.ToString().c_str());
		if ((size_t)ret == _sendbuffer.Size())
		{
			_sendbuffer.Clear();
		}
		else
		{
			_sendbuffer.Copy(_sendbuffer.GetBegin(), _sendbuffer.GetBegin() + ret, _sendbuffer.Size() - ret);
			_sendbuffer.Resize(_sendbuffer.Size() - ret);
		}

		if (_sendbuffer.Empty())
		{
                LOG_DEBUG("netinfo, send data finish, dest:[%s]", _peerAddr.ToString().c_str());
                GetEventManager()->RemoveHandler(base::WriteMask, this);
		}
		else
		{
			timeval tv = {_networkOption->timeout, 0};
            if (GetEventManager()->RegisterHandler(base::WriteMask, this, &tv) == -1)
			{
                LOG_ERROR("register write event fail, client:[%s]", _peerAddr.ToString().c_str());
				Close();
			}
		}
	}
}

void NetMgrEventHandler::HandleTimeout(void *)
{
	

    LOG_ERROR("Connection timeout, remove all events, client:[%s]", _peerAddr.ToString().c_str());
	Close();
}

void NetMgrEventHandler::HandleClose()
{
	LOG_DEBUG("Connection closed");
	::close(GetEventObject().GetObject());
}

void NetMgrEventHandler::Send(const char* data, size_t len)
{
	

	_sendbuffer.Append(data, len);
	HandleOutput();
}

}


