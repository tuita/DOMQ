#ifndef _BASE_NOCOPYABLE_H_
#define _BASE_NOCOPYABLE_H_

namespace base
{

class NoCopyable
{
protected:
    NoCopyable() {}
    ~NoCopyable() {}

private:
    NoCopyable(const NoCopyable &);
    const NoCopyable & operator=(const NoCopyable &);
};

}

#endif
