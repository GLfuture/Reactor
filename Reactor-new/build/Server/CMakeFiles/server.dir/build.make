# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gong/projects/Reactor/Reactor-new

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gong/projects/Reactor/Reactor-new/build

# Include any dependencies generated for this target.
include Server/CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include Server/CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include Server/CMakeFiles/server.dir/flags.make

Server/CMakeFiles/server.dir/server.cc.o: Server/CMakeFiles/server.dir/flags.make
Server/CMakeFiles/server.dir/server.cc.o: ../Server/server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gong/projects/Reactor/Reactor-new/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Server/CMakeFiles/server.dir/server.cc.o"
	cd /home/gong/projects/Reactor/Reactor-new/build/Server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/server.cc.o -c /home/gong/projects/Reactor/Reactor-new/Server/server.cc

Server/CMakeFiles/server.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/server.cc.i"
	cd /home/gong/projects/Reactor/Reactor-new/build/Server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gong/projects/Reactor/Reactor-new/Server/server.cc > CMakeFiles/server.dir/server.cc.i

Server/CMakeFiles/server.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/server.cc.s"
	cd /home/gong/projects/Reactor/Reactor-new/build/Server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gong/projects/Reactor/Reactor-new/Server/server.cc -o CMakeFiles/server.dir/server.cc.s

# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/server.cc.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

../lib/libserver.so: Server/CMakeFiles/server.dir/server.cc.o
../lib/libserver.so: Server/CMakeFiles/server.dir/build.make
../lib/libserver.so: ../lib/libconn.so
../lib/libserver.so: Server/CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gong/projects/Reactor/Reactor-new/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../../lib/libserver.so"
	cd /home/gong/projects/Reactor/Reactor-new/build/Server && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Server/CMakeFiles/server.dir/build: ../lib/libserver.so

.PHONY : Server/CMakeFiles/server.dir/build

Server/CMakeFiles/server.dir/clean:
	cd /home/gong/projects/Reactor/Reactor-new/build/Server && $(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : Server/CMakeFiles/server.dir/clean

Server/CMakeFiles/server.dir/depend:
	cd /home/gong/projects/Reactor/Reactor-new/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gong/projects/Reactor/Reactor-new /home/gong/projects/Reactor/Reactor-new/Server /home/gong/projects/Reactor/Reactor-new/build /home/gong/projects/Reactor/Reactor-new/build/Server /home/gong/projects/Reactor/Reactor-new/build/Server/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Server/CMakeFiles/server.dir/depend

