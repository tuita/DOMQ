#!/bin/sh

for ((i=0;i<10;++i))
do
	python test_protocol.py $i $i & 
done
