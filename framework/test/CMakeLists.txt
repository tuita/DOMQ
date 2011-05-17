INCLUDE_DIRECTORIES(..)
LINK_LIBRARIES(ac netmgr ${LIB_THREAD}  ${LIB_RT})
subdirs(#test_channel  
	#test_connector  
	#test_reactor_multi_process  
	#test_timer
	#test_reactor    
	#test_timeoutqueue
	test_connectionmgr)
