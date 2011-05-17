#include <base/util/util.h>
#include <stdio.h>

namespace base {

std::string Escape(const std::string& s)
{
    std::string result;
    for (size_t i=0; i<s.length(); i++) {
        char c = s[i];
        if (c >= 0x20 && c <= 0x7e) {
            result += c;
        }
        else {
            switch (c) {
            case '\t':
                result += "\\t"; break;
            case '\n':
                result += "\\n"; break;
            case '\r':
                result += "\\r"; break;
            case '\\':
                result += "\\\\"; break;
            case '\0':
                result += "\\0"; break;
            default:
                {
                    char buf[5];
                    snprintf(buf, sizeof(buf), "\\x%02x", (unsigned char)c);
                    result += buf;
                }
            }
        }
    }
    return result;
}

std::string Escape(const char* buf, size_t len)
{
    std::string result;
    for (size_t i=0; i<len; i++) {
        char c = buf[i];
        if (c >= 0x20 && c <= 0x7e) {
            result += c;
        }
        else {
            switch (c) {
            case '\t':
                result += "\\t"; break;
            case '\n':
                result += "\\n"; break;
            case '\r':
                result += "\\r"; break;
            case '\\':
                result += "\\\\"; break;
            case '\0':
                result += "\\0"; break;
            default:
                {
                    char buf[5];
                    snprintf(buf, sizeof(buf), "\\x%02x", (unsigned char)c);
                    result += buf;
                }
            }
        }
    }
    return result;
}

int time_interval(const timeval& t1, const timeval& t2)
{
    return t1.tv_usec - t2.tv_usec + (t1.tv_sec - t2.tv_sec) * 1000000;
}

}

