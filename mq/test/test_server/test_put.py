import sys;
import time;
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

#HOST = "127.0.0.1";
#PORT = 12010;

HOST = "10.241.12.117";
PORT = 12510;


if __name__ == "__main__":

    if len(sys.argv) < 4:
        print "%s queuename data clientname" % sys.argv[0];
        exit();	
    queuename = sys.argv[1];
    data = sys.argv[2];
    clientname = sys.argv[3]; 

    tran = JsonTranslatorEx();
    conn = NetConnection(tran, HOST, PORT);

    req = OpenMsgQueueRequest(queuename, 3, 0, 1, 1, clientname);
    res = conn.sendAndRecvMsg(req);
    print repr(res);
    msgQueueCode = res["msgQueueCode"];
    
    begin =0 ;
    end = 0 ;
    last = 0 ; 
    now = time.time();
    for i in xrange(0, 100000): 
        req = SendMsgRequest(msgQueueCode, data+"_"+str(i));
        try:
            begin = time.time();
            res = conn.sendAndRecvMsg(req);
            end = time.time();
            #print "%f--%f, exec:[%f], data:%s" % (begin, end, end-begin, repr(res));
        except:
            d = datetime.fromtimestamp(begin);
            s = d.strftime('%H:%M:%S');
            sys.stderr.write("[%s]error, %f--%f, exec:[%f], data:%s\n" % (s, begin, end, end-begin, repr(req)));
            conn = NetConnection(tran, HOST, PORT);
        if i%1000 == 0 :
            now = time.time();
            print "now:%f, %f qps" % (now, 1000/(now-last));
            last = now ;
        time.sleep(0.001);
        
    
    
    
    conn.close();
        


