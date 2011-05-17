import sys, time; 
from translator import ProtocolTranslator, StandTranslator;
from msgfactory import MsgFactory;
from addmsg import AddRequest, AddResult;
import network ;


HOST = "127.0.0.1";
PORT = 12010;

MsgFactory.register("AddRequest", AddRequest);
MsgFactory.register("AddResult",  AddResult);

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "%s param1 param2" % sys.argv[0];
        exit();
    param1, param2= int(sys.argv[1]), int(sys.argv[2]);
    
    req = AddRequest();
    req.iSequenceID = 1234;
    req.param1 = param1;
    req.param2 = param2;
    
    tran = ProtocolTranslator();
    conn = network.NetConnection(tran, HOST, PORT);
    res = conn.sendAndRecvMsg(req);
    conn.close();
    print repr(res);
