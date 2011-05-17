#include "queue.h"
#include "msg/msgqueuemsg.h"
#include <string>
#include <sstream>

std::string Queue::ToString() const
{
    std::stringstream st;
    st  << "queuename:[" << option.msgQueueName <<"]," 
        << "msgType:[" << option.msgType << "],"
        << "duration:[" << option.duration << "],"
        << "redisAddr:[" << redisAddr.ToString() << "],"
        << "lastVisit:[" << lastVisit << "]"
        << "maxCount:[" << option.maxCount << "]"
        << "msgCount:[" << msgCount << "]";
    if (option.msgType == SUB2PUB)
    {
        st << "client:[";
        for (std::vector<SubQueue>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            st << "(" << it->client << ',' << it->lastVisit << ',' << it->msgCount << ")";
        }
        st <<"]";
    }

    return st.str();
}

std::string QueueClient::ToString() const
{
    std::stringstream st;
    st  << "msgQueueCode:[" << msgQueueCode << "],"
        << "clientName:["   << clientName   << "],"
        << "queueName:["    << queueName    << "]";
    return st.str();
}
