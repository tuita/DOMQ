#include <redisconnector.h>
#include <ac/network/sockaddr.h>
#include <ac/log/log.h>
#include <ac/log/ostreamlogger.h>
#include <cassert>
#include <unistd.h>
#include <sstream>
#include <signal.h>
#include <ac/util/stringutil.h>
#include <ac/util/scope_guard.h>

std::string PREELEMENT1 = "商报北京消息（山东商报-山东新闻网特派记者 孙珂 冀强）保障房是目前各界讨论的最热话题之一，保障房如何建？建多少？后续如何使用、运营，都是各界民众关心的焦点。保障房建设量首超商品房住建部副部长齐骥昨日在两会记者会上说去年全国销售商品住房9.3亿平方米，算下来大约900万套。今年，会建设1000万套保障性住房，这是国家头一次在住房供应方面，保障类的住房超过市场类住房的总量。从资金来源上说，在拟定1000万套保障性安居工程的目标时，年度投资大概在1.3万亿之间，其中有8000多亿是通过社会机构的投入和保障对象以及所在的企业筹集来的。剩余的5000多亿的资金，将由中央政府和省级人民政府以及市县政府通过各种渠道来筹集。据齐骥介绍，很快会制定金融机构支持保障性安居工程建设特别是公共租赁住房建设的中长期贷款政策。对于公共租赁住房的投资建设，将采取政府主导、社会参与这样一个模式，制定了相应的优惠政策，鼓励和吸引更多的企业参与公共租赁住房的建设和运营。住房租金上涨快政府将加力调控齐骥介绍说，今年各级政府所要拿出的5000多亿的资金，主要是用于160多万套廉租住房和近220万套公共租赁住房的建设。其中近220万套公共租赁住房，大部分集中在沿海城市和外来就业人员比较集中的省会城市和中心城市。对于很多外来务工人员对目前房屋租金持续上涨的反映， 沈建忠说租金的调控也会是房价调控的一部分，政府将加大这方面的调控力度。用公租房满足租赁需求中介哄抬租价将被严查最近，国内部分城市房屋租金上涨比较明显，租户普遍感到经济压力加大，对此房地产市场监管司的沈建忠司长介绍说，目前热点城市、重点城市、大城市租金上涨厉害，从房型上看中小套型的普通住宅房租价格也有所上涨，现在一线的大城市大部分达到500：1。对此，沈建忠认为租金的调控应该也是房价调控的一部分。同时会加大供应，除了我们廉租房要加大建设力度以外，今年要大规模地建公租房。这样就会解决相当一部分租赁需求，特别是中低收入群体的需求。“鼓励一些企业，特别是一些专业性的公司来做出租住房的经营业务。”据悉，今年住建部将进一步加强市场监管。《房地产经纪管理办法》和《商品房屋租赁管理办法》已经出台，这两个办法都对出租房的管理、中介机构、中介人员进行了规定，发现有哄抬租价、欺骗老百姓等违规行为的，要严肃查处。并且，目前正在修订的《房地产法》，特别是对租客的权益要给予保障。增加的住房供应要剔除不合理需求在房地产调控方面，住建部副部长齐骥说要缓解住房的供需矛盾，一是要增加普通商品住房的有效供给，二是在需求当中剔除那些不合理的住房需求，使得增加的住房供应的这部分，能够绝大部分去满足合理的需求。最近几年，需求持续走高，这里不乏有一些属于不合理需求。这些买房人，有的是考虑通胀预期，为了增值保值，有的是想通过房地产的交易来得到一些利润。目前，调控的重点就是要把这部分消费者往后排一排。记者了解到，涉及到社会参与公共租赁住房的建设，更多优惠政策将会体现在税费减免，融资成本如何通过贴息或者专项贷款的方式来减轻。据齐骥介绍，根据国家统计局的资料，中国城市居民89%拥有住房，其中约12%是原来的私有住房；有40%多一点是计划经济时代建的宿舍，其他不到40%一点是居民通过市场购买的商品住房。政府如何筹集5000亿资金政府准备从哪里筹集这5000亿元的巨额资金呢？齐骥说，首先，对保障性安居工程建设资金，地方政府要纳入本级财政预算，在财政预算中要列支一部分。今年严格执行土地出让净收益用于保障性住房建设的比例不低于10%的规定。有些地方把用于保障性安居工程建设的土地净收益计提比例提高到20%。“我们还开展了利用公积金贷款支持保障性住房建设的试点工作，去年计划了400多亿元，今年还要有这样一个指标。同时，今年中央代地方发行的2000亿元的国债当中，地方要优先用于保障性住房，特别是公共租赁住房建设。“除此之外，为了确保今年1000万套保障性安居工程资金的落实，我们还提出和建议了一系列财政、金融等支持政策，比如，在清理地方融资平台时，保留了为支持保障性安居工程建设的融资平台，这个融资平台可以继续运作而且要发展。齐骥说";

//std::string PREELEMENT1 = "test";

int main(int ac, char*av[])
{
    signal(SIGPIPE, SIG_IGN);
    AC_SET_DEFAULT_LOGGER(&ac::OStreamLogger::StdoutLogger);
    RedisConnector con;
    ac::SockAddr addr(ac::INetAddr("127.0.0.1", 6379));
    int ret = con.Open(addr, 3*1000, "123456");
    if (ret != 0 )
    {
        AC_ERROR("conn error:[%x, %d]", ret, ret);
        return -1;
    }
    std::string queueName("_subqueue_q1-get1");
    std::stringstream os; 
   /* for (size_t i=0; i<10; ++i)
    {
        os.str("");
        os << PREELEMENT1 << i ;
        redisReply* reply = static_cast<redisReply*>(con.Command("%s %s %b", RedisCmd::LPUSH.c_str(), queueName.c_str(), os.str().c_str(), os.str().size()));
        AC_ON_BLOCK_EXIT(&RedisConnector::FreeReply, con, reply);
        assert(reply!=NULL);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            AC_DEBUG("LPUSH error:[%s]", ac::Escape(reply->str, reply->len).c_str());
        }
        usleep(5*1000);
    }*/

    for (size_t i=0; i<10000; ++i)
    {
        RedisReply* reply = NULL;
        con.AppendCommand("%s %s", RedisCmd::LPOP.c_str(), queueName.c_str());
        if (REDIS_OK != con.GetReply(reply))
        {
            AC_ERROR("get reply error");
            continue;
        }
        AC_ON_BLOCK_EXIT(&RedisConnector::FreeReply, con, reply);
        /*assert(reply->type == REDIS_REPLY_STRING);
        os.str("");
        os << PREELEMENT1 << i ; 
        assert(0 == strncasecmp(os.str().c_str(), reply->str, os.str().size()));*/
        AC_DEBUG("rpop ret:%d", reply->type);
    }

    con.Close();
}
