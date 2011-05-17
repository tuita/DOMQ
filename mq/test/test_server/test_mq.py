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


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "%s open|close|get|put" % sys.argv[0];
        exit();
    
    tran = JsonTranslatorEx();
    conn = NetConnection(tran, HOST, PORT);
    req = 0;
    if sys.argv[1] == 'open':
        if len(sys.argv) < 3:
	    print "%s open queuename" % sys.argv[0];
	    exit();	
        queuename = sys.argv[2];
        req = OpenMsgQueueRequest(queuename, 0, 1, -1, 1);
        res = conn.sendAndRecvMsg(req);
        print repr(res);
    else:
        if len(sys.argv)<3:
            print "%s get msgqueueCode" % sys.argv[0];
            exit();
        msgQueueCode = sys.argv[2];
        
        if  sys.argv[1] == 'get':
            req = GetMsgRequest(msgQueueCode);
        elif sys.argv[1] == 'put':
            if len(sys.argv) < 4:
                print  "%s get msgqueueCode data" % sys.argv[0];
 		exit();
            data = sys.argv[3];
            req = SendMsgRequest(msgQueueCode, data);
            
        res = conn.sendAndRecvMsg(req);
        print repr(res);
    conn.close();
        


