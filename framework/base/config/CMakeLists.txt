INCLUDE_DIRECTORIES(.. ../libeventreactor)
SET (LIBRARY_OUTPUT_PATH ../lib)

ADD_DEFINITIONS(-g -W -Wall -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DAC_HAS_DEBUG) #AC_HAS_TRACE
ADD_LIBRARY(config STATIC
	libconfig.c      
	libconfigcpp.cc  
	scanner.c  
	tinystr.cpp  
	tinyxmlerror.cpp
	grammar.c            
	libconfigcpp.c++  
	scanctx.c        
	strbuf.c   
	tinyxml.cpp  
	tinyxmlparser.cpp
)

INSTALL(DIRECTORY ./ DESTINATION include/libconfig  PATTERN "*.c*" EXCLUDE PATTERN ".svn" EXCLUDE PATTERN "Makefile" EXCLUDE PATTERN "CMakeFiles" EXCLUDE PATTERN "*.txt" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "*.o" EXCLUDE)
INSTALL(TARGETS config ARCHIVE DESTINATION lib)
