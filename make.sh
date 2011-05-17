#!/bin/bash
pwd=`pwd`
find ./ -name "CMakeCache.txt"|xargs rm -rf
find ./ -name "cmake_install.cmake"|xargs rm -rf
find ./ -name "CMakeFiles" |xargs rm -rf
cd ./framework/jsoncpp
python scons.py platform=linux-gcc
mv libs/linux-gcc-*/libjson_linux-gcc-*.a libs/libjson.a
cd ../
tar -zxf libevent-1.3e.tar.gz.gz
installdir=`pwd`
cd libevent-1.3e-src
./configure --prefix="${installdir}/libevent-1.3e/"
make; make install;
cd ../
cmake .
make

cd ../mq/hiredis
make static

cd ..
cmake . 
make
cd $pwd
