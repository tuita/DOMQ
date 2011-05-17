import sys;

class MsgFactory(object):
    
    __name2Class = {};
    __class2Name = {};
    
    @staticmethod
    def create(msgName):
        return MsgFactory.__name2Class[msgName](); 
    
    @staticmethod
    def register(msgName, func):
        MsgFactory.__name2Class[msgName ] = func;
        MsgFactory.__class2Name[func] = msgName;
        
    @staticmethod
    def getClassName(obj):
        return MsgFactory.__class2Name[type(obj)];
        
    
