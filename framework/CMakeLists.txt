cmake_minimum_required(VERSION 2.6)

PROJECT(base)
SET(CMAKE_AR ar)
FIND_PATH(JSON_INC  json/json.h ./jsoncpp/include NO_DEFAULT_ PATH)
FIND_LIBRARY(JSON_LIB libjson.a ./jsoncpp/libs NO_DEFAULT_PATH)
message(STATUS ${JSON_LIB})
FIND_PATH(LIBEVENT_INC event.h ./libevent-1.3e/include NO_DEFAULT_PATH) 
FIND_LIBRARY(LIBEVENT_LIB libevent.a ./libevent-1.3e/lib NO_DEFAULT_PATH)
 ADD_DEFINITIONS(
 -Os -g -W -Wall -Wextra -fPIC
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)
FIND_LIBRARY(LIB_THREAD pthread /usr/lib /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(LIB_RT rt /usr/lib /usr/local/lib NO_DEFAULT_PATH)

SUBDIRS(base netmgr)
