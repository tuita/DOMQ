#ifndef _BASE_UTIL_UTIL_H_
#define _BASE_UTIL_UTIL_H_

#include <string>
#include <sstream>
#include <algorithm>

namespace base {

std::string Escape(const std::string&);

std::string Escape(const char* buf, size_t len);

template <class T>
std::string ToString(const T& v)
{
    std::ostringstream os;
    os << v;
    return os.str();
}

int time_interval(const timeval& t1, const timeval& t2);

} 
#endif

