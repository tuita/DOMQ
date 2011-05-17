#ifndef _base_dispose_h_
#define _base_dispose_h_

namespace base {
class IDispose
{
public:
    virtual ~IDispose(){};
    virtual void Dispose() = 0 ;
};

}
#endif