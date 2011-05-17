import sys;
from msgqueuemsg import OpenMsgQueueRequest, OpenMsgQueueResult, GetMsgRequest, GetMsgResult, SendMsgRequest, SendMsgResult;
from translator import JsonTranslatorEx;
from network import NetConnection;
from msgfactory import MsgFactory;

MsgFactory.register("OpenMsgQueueRequest", OpenMsgQueueRequest);
MsgFactory.register("OpenMsgQueueResult", OpenMsgQueueResult);
MsgFactory.register("GetMsgRequest", GetMsgRequest);
MsgFactory.register("GetMsgResult", GetMsgResult);
MsgFactory.register("SendMsgRequest", SendMsgRequest);
MsgFactory.register("SendMsgResult", SendMsgResult);

HOST = "127.0.0.1";
PORT = 12010;

#HOST = "10.241.12.117";
#PORT = 12510;


if __name__ == "__main__":

    if len(sys.argv) < 3:
        print "%s queuename data" % sys.argv[0];
        exit();	
    queuename = sys.argv[1];
    data = sys.argv[2];
    
    tran = JsonTranslatorEx();
    conn = NetConnection(tran, HOST, PORT);

    req = OpenMsgQueueRequest(queuename, 0, 1, -1, 1);
    res = conn.sendAndRecvMsg(req);
    print repr(res);
    msgQueueCode = res["msgQueueCode"];

    for i in xrange(0, 10000):    
        req = SendMsgRequest(msgQueueCode, data+str(i));
        res = conn.sendAndRecvMsg(req);
        print repr(res);
    for i in xrange(0, 100): 
        req = GetMsgRequest(msgQueueCode);
        res = conn.sendAndRecvMsg(req);
        print repr(res);
        
    
    
    
    conn.close();
        


