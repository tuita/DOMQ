#ifndef _BASE_THREAD_LOCAL_H_
#define _BASE_THREAD_LOCAL_H_

#include <pthread.h>

typedef void(*CLEANUP_FUNC)(void* p);

namespace base {

class ThreadLocal
{
private:
	pthread_key_t key;

public:
	ThreadLocal(CLEANUP_FUNC func);
	int Set(void* p);
	void* Get();
};

}

#endif

