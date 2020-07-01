cmake_minimum_required(VERSION 3.10)

# Add -O0 to remove optimizations when using gcc
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# set the project name
project(process_monitor)

# add the execuptable
add_executable(process_monitor main.c)

# add the PAPI library
find_library(papi_location NAMES libpapi.a)
message(STATUS ${papi_location})
add_library(papi STATIC IMPORTED)
set_target_properties(papi PROPERTIES IMPORTED_LOCATION ${papi_location})

# find and link timer


# find and link pthread
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(process_monitor PUBLIC "-pthread" papi)
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(process_monitor "${CMAKE_THREAD_LIBS_INIT}" papi)
endif()