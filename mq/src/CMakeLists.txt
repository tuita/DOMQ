SET(LIBRARY_OUTPUT_PATH ../lib)
SET(EXECUTABLE_OUTPUT_PATH ../bin)
include_directories(.. .)

add_library(libmq
    msg/msgqueueerrno.cpp
    msg/msgqueuemsg.cpp
    queuemsgfactory.cpp
    queuemsg.cpp
    queue.cpp
    redisconnector.cpp
    queuemgr.cpp  
    queueprocessor.cpp
    util.cpp
    processcontrol.cpp
    )
    
link_directories(../lib ../hiredis)
LINK_LIBRARIES(libmq ${NETMGR_LIB} ${BASE_LIB} hiredis  ${LIBEVENT_LIB} ${JSON_LIB} ${PTHREAD_LIB} uuid ${RUNTIME_LIB})
add_executable(msgqueue
    main.cpp)
