#ifndef _BASE_EVENT_OBJECT_H_
#define _BASE_EVENT_OBJECT_H_

namespace base {

class EventObject
{
public:
    EventObject(int fd = -1):_fd(fd)
	{
	}

    int GetObject(){ return _fd;}
    void SetObject(int fd) { _fd = fd;}

    void SetInvalid(){ SetObject(-1);}
    bool IsValid() const { return _fd >=0; }
protected:
    int _fd;
};

}

#endif

