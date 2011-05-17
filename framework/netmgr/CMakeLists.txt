INCLUDE_DIRECTORIES(.. ../libeventreactor ${JSON_INC})
link_libraries(${JSON_LIB})
SET (LIBRARY_OUTPUT_PATH ../lib)

ADD_DEFINITIONS(-g -W -Wno-deprecated -Wall -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL)
#ADD_DEFINITIONS(-DAC_HAS_DEBUG -DAC_HAS_TRACE)

ADD_DEFINITIONS(-DKEEP_CONNECTION)

ADD_LIBRARY(netmgr STATIC
    networkoption.cpp
    eventhandler.cpp
    acceptor.cpp
    message.cpp
    coder.cpp
    factory.cpp
    msgfactory.cpp
    handlerregistry.cpp
    msgcallback.cpp
    connectionmgr.cpp
    )

INSTALL(DIRECTORY ./ DESTINATION include/netmgr  PATTERN "*.cpp" EXCLUDE PATTERN ".svn" EXCLUDE PATTERN "Makefile" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "*.txt" EXCLUDE PATTERN "*.cmake" EXCLUDE)
INSTALL(TARGETS netmgr ARCHIVE DESTINATION lib)
