# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/janne/C_source_code/application_modeller/process_monitor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/janne/C_source_code/application_modeller/process_monitor/build

# Include any dependencies generated for this target.
include CMakeFiles/process_monitor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/process_monitor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/process_monitor.dir/flags.make

CMakeFiles/process_monitor.dir/main.c.o: CMakeFiles/process_monitor.dir/flags.make
CMakeFiles/process_monitor.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/janne/C_source_code/application_modeller/process_monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/process_monitor.dir/main.c.o"
	/usr/bin/x86_64-linux-gnu-gcc-7 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/process_monitor.dir/main.c.o   -c /home/janne/C_source_code/application_modeller/process_monitor/main.c

CMakeFiles/process_monitor.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/process_monitor.dir/main.c.i"
	/usr/bin/x86_64-linux-gnu-gcc-7 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/janne/C_source_code/application_modeller/process_monitor/main.c > CMakeFiles/process_monitor.dir/main.c.i

CMakeFiles/process_monitor.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/process_monitor.dir/main.c.s"
	/usr/bin/x86_64-linux-gnu-gcc-7 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/janne/C_source_code/application_modeller/process_monitor/main.c -o CMakeFiles/process_monitor.dir/main.c.s

CMakeFiles/process_monitor.dir/main.c.o.requires:

.PHONY : CMakeFiles/process_monitor.dir/main.c.o.requires

CMakeFiles/process_monitor.dir/main.c.o.provides: CMakeFiles/process_monitor.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/process_monitor.dir/build.make CMakeFiles/process_monitor.dir/main.c.o.provides.build
.PHONY : CMakeFiles/process_monitor.dir/main.c.o.provides

CMakeFiles/process_monitor.dir/main.c.o.provides.build: CMakeFiles/process_monitor.dir/main.c.o


# Object files for target process_monitor
process_monitor_OBJECTS = \
"CMakeFiles/process_monitor.dir/main.c.o"

# External object files for target process_monitor
process_monitor_EXTERNAL_OBJECTS =

process_monitor: CMakeFiles/process_monitor.dir/main.c.o
process_monitor: CMakeFiles/process_monitor.dir/build.make
process_monitor: /usr/local/lib/libpapi.a
process_monitor: CMakeFiles/process_monitor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/janne/C_source_code/application_modeller/process_monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable process_monitor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/process_monitor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/process_monitor.dir/build: process_monitor

.PHONY : CMakeFiles/process_monitor.dir/build

CMakeFiles/process_monitor.dir/requires: CMakeFiles/process_monitor.dir/main.c.o.requires

.PHONY : CMakeFiles/process_monitor.dir/requires

CMakeFiles/process_monitor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/process_monitor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/process_monitor.dir/clean

CMakeFiles/process_monitor.dir/depend:
	cd /home/janne/C_source_code/application_modeller/process_monitor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/janne/C_source_code/application_modeller/process_monitor /home/janne/C_source_code/application_modeller/process_monitor /home/janne/C_source_code/application_modeller/process_monitor/build /home/janne/C_source_code/application_modeller/process_monitor/build /home/janne/C_source_code/application_modeller/process_monitor/build/CMakeFiles/process_monitor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/process_monitor.dir/depend

