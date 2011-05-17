cmake_minimum_required(VERSION 2.6)

FIND_PATH(FRAMEWORK_HOME base/log/log.h  ../framework ~/server/framework /usr/local/framework NO_DEFAULT_PATH)

SET(LIBRARY_OUTPUT_PATH lib)
SET(CMAKE_AR ar)

FIND_PATH(EVENT_INC event.h ../framework/libevent-1.3e/include NO_DEFAULT_PATH)

FIND_PATH(JSON_INC json/json.h ../framework/jsoncpp/include NO_DEFAULT_PATH)
FIND_LIBRARY(BASE_LIB base ${FRAMEWORK_HOME}/lib  NO_DEFAULT_PATH)
FIND_LIBRARY(NETMGR_LIB netmgr ${FRAMEWORK_HOME}/lib  NO_DEFAULT_PATH)

FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(LIBEVENT_LIB event  ../framework/libevent-1.3e/lib  NO_DEFAULT_PATH)
FIND_LIBRARY(JSON_LIB libjson.a ../framework/jsoncpp/libs NO_DEFAULT_PATH)

INCLUDE_DIRECTORIES(src  ${FRAMEWORK_HOME}  ${EVENT_INC} ${JSON_INC})


ADD_DEFINITIONS(
 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -O2  -fPIC  
 -D_REENTRANT 
)


SUBDIRS(src)
