from msg import JsonMsg;


g_msgid= 0 ;

def genMsgNo():
    global g_msgid;
    g_msgid = g_msgid +1; 
    return g_msgid; 

class OpenMsgQueueRequest(JsonMsg):
    def __init__(self, msgQueueName, create, option, duration, role, clientName):
        self.serialNo = 12;
        self.msgQueueName = msgQueueName;
        self.create = create;
        self.option = option;
        self.duration = duration;
        self.role = role ;
        self.clientName = clientName;

class OpenMsgQueueResult(JsonMsg):
    def __init__(self):
        self.serialNo = 0;
        self.msgQueueCode = "";
        
class GetMsgRequest(JsonMsg):
    def __init__(self, msgQueueCode):
        self.serialNo = genMsgNo();
        self.msgQueueCode = msgQueueCode;
        self.autoReply = 1;
        
class GetMsgResult(JsonMsg):
    def __init__(self):
        self.serialNo = 0;
        self.msgNo = 0;
        self.data = "";
        
class SendMsgRequest(JsonMsg):
    def __init__(self, msgQueueCode, data):
        self.serialNo = genMsgNo();
        self.msgQueueCode = msgQueueCode;
        self.msgData = data;
        
class SendMsgResult(JsonMsg):
    def __init__(self):
        self.serialNo = 0;
