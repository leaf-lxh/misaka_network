# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build

# Include any dependencies generated for this target.
include CMakeFiles/tinyHttpd.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tinyHttpd.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tinyHttpd.dir/flags.make

CMakeFiles/tinyHttpd.dir/example.cpp.o: CMakeFiles/tinyHttpd.dir/flags.make
CMakeFiles/tinyHttpd.dir/example.cpp.o: ../example.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tinyHttpd.dir/example.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyHttpd.dir/example.cpp.o -c /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/example.cpp

CMakeFiles/tinyHttpd.dir/example.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyHttpd.dir/example.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/example.cpp > CMakeFiles/tinyHttpd.dir/example.cpp.i

CMakeFiles/tinyHttpd.dir/example.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyHttpd.dir/example.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/example.cpp -o CMakeFiles/tinyHttpd.dir/example.cpp.s

CMakeFiles/tinyHttpd.dir/example.cpp.o.requires:

.PHONY : CMakeFiles/tinyHttpd.dir/example.cpp.o.requires

CMakeFiles/tinyHttpd.dir/example.cpp.o.provides: CMakeFiles/tinyHttpd.dir/example.cpp.o.requires
	$(MAKE) -f CMakeFiles/tinyHttpd.dir/build.make CMakeFiles/tinyHttpd.dir/example.cpp.o.provides.build
.PHONY : CMakeFiles/tinyHttpd.dir/example.cpp.o.provides

CMakeFiles/tinyHttpd.dir/example.cpp.o.provides.build: CMakeFiles/tinyHttpd.dir/example.cpp.o


CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o: CMakeFiles/tinyHttpd.dir/flags.make
CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o: ../tinyHttpd.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o -c /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/tinyHttpd.cpp

CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/tinyHttpd.cpp > CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.i

CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/tinyHttpd.cpp -o CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.s

CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.requires:

.PHONY : CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.requires

CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.provides: CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.requires
	$(MAKE) -f CMakeFiles/tinyHttpd.dir/build.make CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.provides.build
.PHONY : CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.provides

CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.provides.build: CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o


CMakeFiles/tinyHttpd.dir/webstring.cpp.o: CMakeFiles/tinyHttpd.dir/flags.make
CMakeFiles/tinyHttpd.dir/webstring.cpp.o: ../webstring.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/tinyHttpd.dir/webstring.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyHttpd.dir/webstring.cpp.o -c /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/webstring.cpp

CMakeFiles/tinyHttpd.dir/webstring.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyHttpd.dir/webstring.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/webstring.cpp > CMakeFiles/tinyHttpd.dir/webstring.cpp.i

CMakeFiles/tinyHttpd.dir/webstring.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyHttpd.dir/webstring.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/webstring.cpp -o CMakeFiles/tinyHttpd.dir/webstring.cpp.s

CMakeFiles/tinyHttpd.dir/webstring.cpp.o.requires:

.PHONY : CMakeFiles/tinyHttpd.dir/webstring.cpp.o.requires

CMakeFiles/tinyHttpd.dir/webstring.cpp.o.provides: CMakeFiles/tinyHttpd.dir/webstring.cpp.o.requires
	$(MAKE) -f CMakeFiles/tinyHttpd.dir/build.make CMakeFiles/tinyHttpd.dir/webstring.cpp.o.provides.build
.PHONY : CMakeFiles/tinyHttpd.dir/webstring.cpp.o.provides

CMakeFiles/tinyHttpd.dir/webstring.cpp.o.provides.build: CMakeFiles/tinyHttpd.dir/webstring.cpp.o


# Object files for target tinyHttpd
tinyHttpd_OBJECTS = \
"CMakeFiles/tinyHttpd.dir/example.cpp.o" \
"CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o" \
"CMakeFiles/tinyHttpd.dir/webstring.cpp.o"

# External object files for target tinyHttpd
tinyHttpd_EXTERNAL_OBJECTS =

tinyHttpd: CMakeFiles/tinyHttpd.dir/example.cpp.o
tinyHttpd: CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o
tinyHttpd: CMakeFiles/tinyHttpd.dir/webstring.cpp.o
tinyHttpd: CMakeFiles/tinyHttpd.dir/build.make
tinyHttpd: CMakeFiles/tinyHttpd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable tinyHttpd"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyHttpd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tinyHttpd.dir/build: tinyHttpd

.PHONY : CMakeFiles/tinyHttpd.dir/build

CMakeFiles/tinyHttpd.dir/requires: CMakeFiles/tinyHttpd.dir/example.cpp.o.requires
CMakeFiles/tinyHttpd.dir/requires: CMakeFiles/tinyHttpd.dir/tinyHttpd.cpp.o.requires
CMakeFiles/tinyHttpd.dir/requires: CMakeFiles/tinyHttpd.dir/webstring.cpp.o.requires

.PHONY : CMakeFiles/tinyHttpd.dir/requires

CMakeFiles/tinyHttpd.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tinyHttpd.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tinyHttpd.dir/clean

CMakeFiles/tinyHttpd.dir/depend:
	cd /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build /mnt/d/ProgrammingCode/C++/misaka_network/tinyHttpd/build/CMakeFiles/tinyHttpd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tinyHttpd.dir/depend

