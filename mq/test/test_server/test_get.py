import sys, time;
from datetime import datetime;
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
COUNT = 1000*1000;

#HOST = "10.241.12.117";
#PORT = 12510;


if __name__ == "__main__":

    if len(sys.argv) < 3:
        print "%s queuename clientname" % sys.argv[0];
        exit();	
    queuename = sys.argv[1];
    clientname = sys.argv[2];
    
    tran = JsonTranslatorEx();
    conn = NetConnection(tran, HOST, PORT);
    req = OpenMsgQueueRequest(queuename, 0, 0, -1, 0, clientname);
    res = conn.sendAndRecvMsg(req);
    msgQueueCode = res["msgQueueCode"];

    #for i in xrange(0, 10000):    
    #    req = SendMsgRequest(msgQueueCode, data+str(i));
    #    res = conn.sendAndRecvMsg(req);
    #    print repr(res);
    last = 0 ;
    now = time.time();
    for i in xrange(0, COUNT): 
       req = GetMsgRequest(msgQueueCode);
       try:
           begin = time.time();
           res = conn.sendAndRecvMsg(req);
           end = time.time(); 
       except :
            d = datetime.fromtimestamp(begin);
            s = d.strftime('%H:%M:%S');
            sys.stderr.write("[%s]error, %f--%f, exec:[%f], data:%s\n" % (s, begin, end, end-begin, repr(req)));
            conn = NetConnection(tran, HOST, PORT);
       if i%1000 == 0:
            now = time.time();
            print "now:%f, %f qps" % (now, 1000/(now-last));
            last = now;
       time.sleep(0.001);
    conn.close(); 
        


