#ifndef _BASE_CALLBACK_H_
#define _BASE_CALLBACK_H_

namespace base {

struct CallBack
{
	virtual ~CallBack() {}
	virtual int Call(void* p) = 0;
};

}
#endif

