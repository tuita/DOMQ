# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/u2/workstation/src/addons/server/opensource/mq

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/u2/workstation/src/addons/server/opensource/mq

# Include any dependencies generated for this target.
include src/CMakeFiles/msgqueue.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/msgqueue.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/msgqueue.dir/flags.make

src/CMakeFiles/msgqueue.dir/main.cpp.o: src/CMakeFiles/msgqueue.dir/flags.make
src/CMakeFiles/msgqueue.dir/main.cpp.o: src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/u2/workstation/src/addons/server/opensource/mq/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/msgqueue.dir/main.cpp.o"
	cd /home/u2/workstation/src/addons/server/opensource/mq/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/msgqueue.dir/main.cpp.o -c /home/u2/workstation/src/addons/server/opensource/mq/src/main.cpp

src/CMakeFiles/msgqueue.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/msgqueue.dir/main.cpp.i"
	cd /home/u2/workstation/src/addons/server/opensource/mq/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/u2/workstation/src/addons/server/opensource/mq/src/main.cpp > CMakeFiles/msgqueue.dir/main.cpp.i

src/CMakeFiles/msgqueue.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/msgqueue.dir/main.cpp.s"
	cd /home/u2/workstation/src/addons/server/opensource/mq/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/u2/workstation/src/addons/server/opensource/mq/src/main.cpp -o CMakeFiles/msgqueue.dir/main.cpp.s

src/CMakeFiles/msgqueue.dir/main.cpp.o.requires:
.PHONY : src/CMakeFiles/msgqueue.dir/main.cpp.o.requires

src/CMakeFiles/msgqueue.dir/main.cpp.o.provides: src/CMakeFiles/msgqueue.dir/main.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/msgqueue.dir/build.make src/CMakeFiles/msgqueue.dir/main.cpp.o.provides.build
.PHONY : src/CMakeFiles/msgqueue.dir/main.cpp.o.provides

src/CMakeFiles/msgqueue.dir/main.cpp.o.provides.build: src/CMakeFiles/msgqueue.dir/main.cpp.o
.PHONY : src/CMakeFiles/msgqueue.dir/main.cpp.o.provides.build

# Object files for target msgqueue
msgqueue_OBJECTS = \
"CMakeFiles/msgqueue.dir/main.cpp.o"

# External object files for target msgqueue
msgqueue_EXTERNAL_OBJECTS =

bin/msgqueue: src/CMakeFiles/msgqueue.dir/main.cpp.o
bin/msgqueue: lib/liblibmq.a
bin/msgqueue: /home/u2/workstation/src/addons/server/opensource/framework/lib/libnetmgr.a
bin/msgqueue: /home/u2/workstation/src/addons/server/opensource/framework/lib/libbase.a
bin/msgqueue: /usr/lib/libevent.so
bin/msgqueue: /home/u2/workstation/src/addons/server/apipoold/trunk/lib/libjson.a
bin/msgqueue: /usr/lib/libpthread.so
bin/msgqueue: /usr/lib/librt.so
bin/msgqueue: src/CMakeFiles/msgqueue.dir/build.make
bin/msgqueue: src/CMakeFiles/msgqueue.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../bin/msgqueue"
	cd /home/u2/workstation/src/addons/server/opensource/mq/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/msgqueue.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/msgqueue.dir/build: bin/msgqueue
.PHONY : src/CMakeFiles/msgqueue.dir/build

src/CMakeFiles/msgqueue.dir/requires: src/CMakeFiles/msgqueue.dir/main.cpp.o.requires
.PHONY : src/CMakeFiles/msgqueue.dir/requires

src/CMakeFiles/msgqueue.dir/clean:
	cd /home/u2/workstation/src/addons/server/opensource/mq/src && $(CMAKE_COMMAND) -P CMakeFiles/msgqueue.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/msgqueue.dir/clean

src/CMakeFiles/msgqueue.dir/depend:
	cd /home/u2/workstation/src/addons/server/opensource/mq && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/u2/workstation/src/addons/server/opensource/mq /home/u2/workstation/src/addons/server/opensource/mq/src /home/u2/workstation/src/addons/server/opensource/mq /home/u2/workstation/src/addons/server/opensource/mq/src /home/u2/workstation/src/addons/server/opensource/mq/src/CMakeFiles/msgqueue.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/msgqueue.dir/depend

