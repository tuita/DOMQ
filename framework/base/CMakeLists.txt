
INCLUDE_DIRECTORIES(.. ${LIBEVENT_INC})
SET (LIBRARY_OUTPUT_PATH ../lib)

ADD_DEFINITIONS(-g -W -Wall -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -D_MULTI_THREAD -DNETMGRLOG) 
ADD_LIBRARY(base STATIC
    log/log.cpp
    
    third/md5.cpp
    
    thread/mutex.cpp
    thread/condition.cpp
    thread/thread.cpp
    thread/threadpool.cpp
    thread/threadlocal.cpp
    
    util/allocator.cpp
    util/memblock.cpp
    util/util.cpp
    third/md5.cpp
    
    net/sockaddr.cpp
    net/socket.cpp
    net/acceptor.cpp
    net/eventmanager.cpp
    net/pipeeventhandler.cpp
    
    config/libconfig.c      
    config/libconfigcpp.cc  
    config/scanner.c  
    config/tinystr.cpp  
    config/tinyxmlerror.cpp
    config/grammar.c            
    config/libconfigcpp.c++  
    config/scanctx.c        
    config/strbuf.c   
    config/tinyxml.cpp  
    config/tinyxmlparser.cpp
)

INSTALL(DIRECTORY encoding network util log DESTINATION include/ac  PATTERN "*.cpp" EXCLUDE PATTERN ".svn" EXCLUDE)
INSTALL(TARGETS base ARCHIVE DESTINATION lib)
