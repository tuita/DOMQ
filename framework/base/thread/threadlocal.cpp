#include <base/thread/threadlocal.h>
#include <exception>

namespace base {

ThreadLocal::ThreadLocal(CLEANUP_FUNC f)
{
	pthread_key_create(&key, f);
}

int ThreadLocal::Set(void* p)
{
	return pthread_setspecific(key, p);
}

void* ThreadLocal::Get()
{
	return pthread_getspecific(key);
}

} // namespace ac

