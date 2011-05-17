import socket, struct;

#HOST = '127.0.0.1'
#PORT = 11000              # The same port as used by the server
BUFFER_LEN = 10*1024*1024;
DEBUG=True;
DEBUG_BINARY = False;

def debug(data):
    if not DEBUG:
        return; 
    if DEBUG_BINARY:
        print ' '.join(["%x" % (ord(x)) for x in data]);
    else:
        print repr(data);
class NetConnection(object):
    
    def __init__(self, trans, host, port):
        self.trans = trans; 
        if type(port) == int:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
            self.s.connect((host, port));
        else:
            self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM); 
            self.s.connect(port);
    
    def close(self):
        self.s.close();
    
    def recvn(self, n):
        offset=0 ; 
        buffer=[];
        while offset < n:
            data = self.s.recv(n-offset);
            buffer.append(data);
            offset = offset+len(data);
        
        return ''.join(buffer);
            
            
        
    def sendAndRecv(self, data):
        #s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
        #s.connect((host, port));
        self.s.send(data);
        data = self.recvn(4);
        (datalen,) = struct.unpack_from('>I', data);
        data = data + self.recvn(datalen-4);
        #s.close();
        return data;
        
    def sendAndRecvMsg(self, req):
        msg = self.trans.encode(req);
        debug(msg);
        recvData = self.sendAndRecv(msg);
        debug(recvData);
        res = self.trans.decode(recvData);
        return res;
        
    def send(self, tran, req):
        msg = self.trans.encode(req);
        debug(msg);
        recvData = self.sendAndRecv(msg);
        debug(recvData);
        res = self.trans.decode(recvData);
        return res;
