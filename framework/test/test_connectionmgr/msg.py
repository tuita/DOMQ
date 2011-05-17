import sys;
import json;
from google.protobuf.message import Message;
from google.protobuf import text_format


class Msg(object):
    
    def __init__(self):
        self.pmsg = Message;
        
    def __getattr__(self, key):
        if self.__dict__.has_key(key):
            return self.__dict__[key];
        return getattr(self.pmsg, key);
        
    def __setattr__(self, key, value):
        if self.__dict__.has_key(key) or key == "pmsg":
            self.__dict__[key] = value;
        else:
            self.pmsg.__setattr__(key, value);
        
    def encode(self):
        return self.pmsg.SerializeToString();
        
    def decode(self, data):
        return self.pmsg.ParseFromString( data);
        
    def __repr__(self):
        return text_format.MessageToString(self.pmsg);
        
class JsonMsg(object):
    
    def jsonEncode(self):
        pass;
    
    def jsonDecode(self, data):
        self.data = json.loads(data);
        
    def __repr__(self):
        return json.dumps(self.data);
    
if __name__ == "__main__":
    
    msg = Msg();
    msg.serialNo = 30;
    msg2 = Msg();
    msg2.decode(msg.encode());
    print repr(msg.encode());
    if( msg.serialNo != msg2.serialNo):
        print "test fail";
        exit;
    print "test succ";
       
   
