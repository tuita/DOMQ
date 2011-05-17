INCLUDE_DIRECTORIES(. ../../..  ../../libeventreactor)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DAC_HAS_DEBUG) #-DAC_HAS_TRACE
LINK_LIBRARIES(libeventreactor  config ac pthread dl ${LIBEVENT_LIB} ${PROTOC_LIB})
LINK_DIRECTORIES(../../lib)
ADD_EXECUTABLE(test_connectionmgr
    addmsg.cpp
    msgfactory.cpp
    pmsg/addmsg.pb.cc
	test_connectionmgr.cpp)
