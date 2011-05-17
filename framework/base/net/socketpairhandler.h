#ifndef _NETMGR_SOCKET_PAIR_HANDLER_H
#define _NETMGR_SOCKET_PAIR_HANDLER_H

#include <stdio.h>
#include <base/net/eventhandler.h>


namespace base{

class CallBack;

class SocketPairHandler : public EventHandler
{
public:
    SocketPairHandler(CallBack* callback = NULL):_callback(callback){}
	int GetAnotherHandle() const { return _anotherhandle; }

	virtual int Open();
	virtual int Close(){ return 0;};
    void SetEventCallBack(CallBack* callback){ _callback = callback;}

protected:
	virtual void HandleInput();
	virtual void HandleClose();
	virtual void HandleOutput(){};
    void HandleTimeout(void * /*pTimerID*/){}

private:
	void CloseHandles();

private:
	int _anotherhandle;
    CallBack* _callback;
};

} // namespace base


#endif // AC_SOCKET_PAIR_HANDLER_H

