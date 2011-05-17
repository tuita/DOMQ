#include <netmgr/connectionmgr.h>
#include <netmgr/acceptor.h>
#include <netmgr/coder.h>
#include <base/net/eventmanager.h>
#include <base/log/log.h>
#include <base/third/scope_guard.h>
#include <base/util/util.h>
#include <errno.h>
#include <unistd.h>
#include <cassert>
#include <list>


__thread char * _connectionMgrPacketBuf = NULL;
__thread size_t _connectionMgrPacketBufSize = 1024*1024 ;
namespace netmgr
{

ConnectionThread::ConnectionThread():_eventMgr(NULL)
{ 
}

void ConnectionThread::Run()
{
    assert(_eventMgr);
    _eventMgr->Run();
}
ConnectionMgr::ConnectionMgr()
: _eventMgr(NULL)
{
}

ConnectionMgr::~ConnectionMgr()
{
    
}

int ConnectionMgr::Open(libconfig::Setting& setting)
{
    _requestLimit = size_t((int)setting["QueueLimit"]);
    _resultLimit = size_t((int)setting["PackageLimit"]);
    _eventMgr = new base::EventManager;
    _connectionThread.SetReactor(_eventMgr);

    _extractMsgCallBack.SetQueue(&_requestQueue);

	_newMsgCallBack.SetQueue(&_resultQueue);
	_newMsgCallBack.SetHandlerRegistry(&_eventhandleRegistery);
    
    _newMsgEventHandler.SetEventManager(_eventMgr);
    _newMsgEventHandler.SetEventCallBack(&_newMsgCallBack);
	if (_newMsgEventHandler.Open() != 0)
	{
		LOG_ERROR("pushhandler open fail");
		return -1;
	}
    _queuePushMsgCallBack.SetPipe(_newMsgEventHandler.GetAnotherHandle());
	_resultQueue.SetPushCallBack(&_queuePushMsgCallBack);

    libconfig::Setting& acceptSetting = setting["NetworkAcceptors"];
    if( 0 != InitNetwork(acceptSetting))
    {
        return -1;
    }

	return 0;
}

int ConnectionMgr::ParseCoder(NetworkOption& option, const libconfig::Setting& setting)
{
    std::string serviceName = setting.exists("ServiceName") ? (const char*)setting["ServiceName"] : "";
    std::string coderName = setting.exists("Coder") ? (const char*)setting["Coder"] : "";
    switch (option.protocol)
    {
    case JSON_PROTOCOL:
        {
            if (coderName == "")
            {
                option.coder = JsonCoder::Instance();
            }
            else if (coderName == "JSONRPC_PROTOCOL")
            {
                option.coder = new JsonRpcCoder;
                _coders.push_back(option.coder);
            }
            else
            {
                LOG_ERROR("JSON_PROTOCOL, TRANSLATORNAME:[%s] is not implement", coderName.c_str());
            }
            
            break;
        }
    case RAWDATA_PROTOCOL:
        {
            option.coder = LineCoder::Instance() ; 
            break;
        }
    default:
        LOG_ERROR("protocol error.only support PROTOCOLBUF_PROTOCOL|STAND_PROTOCOL|XCODE_PROTOCOL");
        return -1;
    }
    
    if (coderName == "SERVICEFLAGTRANSLATOR")
    {
        ServiceFlagCoder* coder = new ServiceFlagCoder(base::NilMutex::Instance());
        coder->SetCoder(option.coder);
        _coders.push_back(coder);
        option.coder = coder;
        if (!setting.exists("InternalCoder"))
        {
            LOG_ERROR("ServiceFlagCoder InternalCoder should set");
            return -1;
        }
        int internalProtocol = ParseProtocol((const char*)setting["InternalProtocol"]);
        if (internalProtocol == JSON_PROTOCOL)
        {
            coder->SetInternalCoder(JsonCoder::Instance());
        }
        else
        {
            LOG_WARN("protocol [%x], translator:[%s] is error", option.protocol, coderName.c_str());
            return -1;
        }
    }
    option.coder->SetExtractMsgCallBack(&_extractMsgCallBack);
    return 0;
}

int ConnectionMgr::InitNetwork(libconfig::Setting& acceptSetting)
{
    for ( int i=0; i<acceptSetting.getLength(); ++i ) 
    {
        NetworkOption stNetworkOption;
        stNetworkOption.address.SetAddr(acceptSetting[i]["Address"]);
        
        std::string translatorName = acceptSetting[i].exists("Coder") ? (const char*)acceptSetting[i]["Coder"] : "";
        stNetworkOption.protocol = ParseProtocol( acceptSetting[i].exists("Protocol") ? (const char*)acceptSetting[i]["Protocol"] : "STAND_PROTOCOL");
        LOG_INFO("open acceptor %s, protocol:%x, translator:%s", (const char*)acceptSetting[i]["Address"], stNetworkOption.protocol, translatorName.c_str());
        if(0 != ParseCoder(stNetworkOption, acceptSetting[i]))
        {
            LOG_ERROR("parseCoder error, protocol:[%x], serviceName:[%s]", stNetworkOption.protocol, (const char*)acceptSetting[i]["ServiceName"]);  
            return -1;
        }
        if ( acceptSetting[i].exists("Timeout") ) 
        {
            stNetworkOption.timeout = acceptSetting[i]["Timeout"];
        }
        if ( acceptSetting[i].exists("FirstTimeout") ) 
        {
            stNetworkOption.firstTimeout = acceptSetting[i]["FirstTimeout"];
        }
        if ( acceptSetting[i].exists("SendBufSize") ) 
        {
            stNetworkOption.sendBufSize = acceptSetting[i]["SendBufSize"];
        }
        if ( acceptSetting[i].exists("RecvBufSize") ) 
        {
            stNetworkOption.recvBufSize = acceptSetting[i]["RecvBufSize"];
        }

        if( 0 != Listen(stNetworkOption))
        {
            LOG_ERROR("listen addr:[%s] error", stNetworkOption.address.ToString().c_str());
            return -1;
        }
    }

    for(std::list<base::Acceptor*>::iterator it = _acceptors.begin(); it!= _acceptors.end(); ++it)
    {
        (*it)->SetEventManager(_eventMgr);
        if ((*it)->Open() != 0)
        {
            LOG_ERROR("acceptor open fail: %s", strerror(errno));
            return -1;
        }
    } 
    return 0;
}

void ConnectionMgr::Close()
{
	for(std::list<base::Acceptor*>::iterator it = _acceptors.begin(); it!= _acceptors.end(); ++ it)
	{
		(*it)->Close();
	}

	::close(_newMsgEventHandler.GetAnotherHandle());
	_newMsgEventHandler.GetEventObject().SetInvalid();
	::close(_newMsgEventHandler.GetEventObject().GetObject());

	for(std::list<base::Acceptor*>::iterator it = _acceptors.begin(); it!= _acceptors.end(); ++ it)
	{
		delete (*it);
	}
	_acceptors.clear();

	delete _eventMgr;

    for (std::vector<netmgr::ICoder*>::iterator it = _coders.begin(); it != _coders.end(); ++it)
    {
        delete *it;
    }
    
	_eventMgr = NULL;
}

void ConnectionMgr::Run(bool Sync)
{
	
    _connectionThread.Start();
    if (Sync)
    {
        _connectionThread.Wait();
    } 
}

void ConnectionMgr::Stop()
{
	
    _eventMgr->Stop();
    _connectionThread.Stop();
	
}



char* ConnectionMgr::GetBuffer(size_t bufSize)
{
    if (bufSize < _connectionMgrPacketBufSize || NULL == _connectionMgrPacketBuf)
    {
        delete _connectionMgrPacketBuf;
        _connectionMgrPacketBuf = new char[bufSize];
    }
    return _connectionMgrPacketBuf;
    
}

size_t ConnectionMgr::GetBufferSize()
{
    return _connectionMgrPacketBufSize;
}

//int ConnectionMgr::PushPacket(Package* pack)
//{  
//    _resultQueue.Push(pack);
//    gettimeofday(&(pack->puttime2), NULL);
//    return true;
//}

Message* ConnectionMgr::PopMsg(bool block)
{
    return PopMsg(block? -1 : 0);
}

Message* ConnectionMgr::PopMsg(int usBlockTime)
{
    netmgr::Message* m;
    return _requestQueue.Get(m, usBlockTime) == 0 ? m: NULL;
}

int ConnectionMgr::PushMsg(Message* msg)
{
	return _resultQueue.Put(msg);
    //NetMgrEventHandler* eventHandler = dynamic_cast<NetMgrEventHandler*>(_eventhandleRegistery.GetHandler(msg->context.peer.addr));
    //if (NULL == eventHandler)
    //{
    //    LOG_ERROR("msg:[%s], content:[%s], not find eventhandler", msg->GetTypeName().c_str(), msg->ToString().c_str());
    //    return -1;
    //}
    //
    //ICoder* coder = eventHandler->GetNetworkOption().coder;
    //if (NULL == coder)
    //{
    //    LOG_ERROR("socket:%zd get translator error, msg:[%s], content:[%s]", 
    //        eventHandler->GetPeerAddr().ToString().c_str(), msg->GetTypeName().c_str(), msg->ToString().c_str());
    //    return -1;
    //}
    //size_t buflen = GetBufferSize();
    //char* buf = GetBuffer(buflen);
    //int  len = coder->Encode(buf, buflen, msg);
    //if (len < 0)
    //{
    //    LOG_ERROR("encode data error, msg:[%s], content:[%s]", msg->GetTypeName().c_str(), msg->ToString().c_str());
    //    return -1;
    //}
    //LOG_DEBUG("msg:%s, encode data:%s", msg->ToString().c_str(), base::Escape(buf, len).c_str());

    //Package* packet = packManager.Create();
    //if (NULL == packet)
    //{
    //    LOG_ERROR("create packet error, msg:[%s], content:[%s]", msg->GetTypeName().c_str(), msg->ToString().c_str());
    //    return -1;
    //}
    //packet->SetAllocator(&_memAllocator);
    //packet->SetData(buf, len);
    ///*packet->SetGlobalId(msg->context.peer.handleId);*/
    //packet->SetPeerAddr(eventHandler->GetPeerAddr());
    //packet->starttime = msg->context.starttime;
    //packet->puttime = msg->context.puttime;
    //packet->gettime = msg->context.gettime;
    ///*packet->processtime = msg->context.processtime;*/
    //gettimeofday(&(packet->processtime), NULL);
    //PushPacket(packet);
    return 0;
}

int ConnectionMgr::Listen(const NetworkOption& option)
{
    netmgr::Acceptor* acceptor = new netmgr::Acceptor(option.address, option, &_eventhandleRegistery);
    _acceptors.push_back(acceptor);
    return 0;
}
}


