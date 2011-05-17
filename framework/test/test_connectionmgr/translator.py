import struct;
import sys;
import msg;
from msgfactory import MsgFactory;

class  Translator(object):
    '''translator msg, the msg must have such properties:
        1:msgName;
        2:encode 
        3:decode'''
        
    def decode(self, msg):
        pass; 
        
    def encode(self, msg):
        pass;
        
class ProtocolTranslator(object):
    '''protocol msg'''
    def encode(self, msg):
        data=''.join((struct.pack('>I'+str(len(x))+'s', len(x), x) for x in (MsgFactory.getClassName(msg), msg.encode())));
        return struct.pack('>I'+str(len(data))+'s', len(data)+4, data);
    
    def decode(self, data):
        (len, msgLen) = struct.unpack_from('>II', data);
        offset = struct.calcsize('>II');
        (msgName, protoLen) = struct.unpack_from('>'+str(msgLen)+'sI', data, offset);
        offset = offset + msgLen+ struct.calcsize('>I');
        msg = MsgFactory.create(msgName);
        #print msg, repr(data[offset:]);
        msg.decode(data[offset:]);
        return msg;
        
class JsonTranslator(object):
    
    def encode(self, msg):
        data=''.join((struct.pack('>I'+str(len(x))+'s', len(x), x) for x in (MsgFactory.getClassName(msg), msg.jsonEncode())));
        return struct.pack('>I'+str(len(data))+'s', len(data)+4, data);
    
    def decode(self, data):
        (len, msgLen) = struct.unpack_from('>II', data);
        offset = struct.calcsize('>II');
        (msgName, protoLen) = struct.unpack_from('>'+str(msgLen)+'sI', data, offset);
        offset = offset + msgLen+ struct.calcsize('>I');
        msg = MsgFactory.create(msgName);
        #print msg, repr(data[offset:]);
        msg.jsonDecode(data[offset:]);
        return msg;

class  ServiceTranslator(object):
    def __init__(self, externalTrans, internalTrans):
        self._translator = externalTrans;
        self._internalTranslator = internalTrans;
    
    
class StandTranslator(object):
    '''stand translator, default, '''
    
    def decode(self, data):
        pass; 
    
    def encode(self, msg):
        if msg.receiverName:
            receiver = struct.pack('>III', msg.receiver.id, msg.receiver.remote, msg.receiver.time);
        else:
            receiver = struct.pack('>I'+str(len(msg.receiverName))+'s', len(msg.receiverName), msg.receiverName);
        sender = struct.pack('>III', msg.sender.id, msg.sender.remote, msg.sender.time);
        msgName = MsgFactory.getClassName(msg);
        msgName = struct.pack(('>I%ds' % len(msgName)), msgName);
        data = msg.encode();
        data = struct.pack(('>I%ds' % len(data)), data);
        alldata = '%s%s%s%s' % (receiver, sender, msgName, data);
        return struct.pack(('>I%ds' % len(alldata)), alldata);
        
if __name__ == "__main__":
    MsgFactory.register("msg", msg.Msg);
    msg = msg.Msg();
    msg.serialNo = 60;
    translator = ProtocolTranslator();
    data = translator.encode(msg);
    print repr(data);
    msg1 = translator.decode(data);
    print msg1.serialNo;
    
    
