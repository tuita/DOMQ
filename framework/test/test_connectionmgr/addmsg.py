from addmsg_pb2 import AddMsgRequest, AddMsgResult;
from msg import Msg;

class AddRequest(Msg):
    def __init__(self):
        self.pmsg = AddMsgRequest();
        
    
        
class AddResult(Msg):
    def __init__(self):
        self.pmsg = AddMsgResult();