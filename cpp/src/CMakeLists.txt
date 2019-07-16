cmake_minimum_required (VERSION 3.7.2)
project(Runtime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0 -fpermissive")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fpermissive")

#set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2")


add_definitions(-DBOOST_ALL_NO_LIB)


set(GLOBAL_DEPENDENCIES_PATH "${PROJECT_SOURCE_DIR}/dependencies/cpp")
set(RUNTIME_PATH "${PROJECT_SOURCE_DIR}/analyst/env/code/cpp")


include_directories(${GLOBAL_DEPENDENCIES_PATH})
include_directories(${RUNTIME_PATH})

#directory for executables
file(MAKE_DIRECTORY "bin")
file(MAKE_DIRECTORY "bin/x64")
file(MAKE_DIRECTORY "bin/x64/Debug")
file(MAKE_DIRECTORY "bin/x64/Release")

#set appropriate folder for executables collection
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(BIN_FOLDER "bin/x64/Release")
	set(LIB_PATH "${PROJECT_SOURCE_DIR}/dependencies/lib/x64/Release")
else()
	set(CMAKE_BUILD_TYPE "Debug")
	set(BIN_FOLDER "bin/x64/Debug")
	set(LIB_PATH "${PROJECT_SOURCE_DIR}/dependencies/lib/x64/Debug")
endif()

link_directories(${LIB_PATH})

string(ASCII 27 Esc)
set(Red "${Esc}[31m")
set(Yellow "${Esc}[33m")
set(Blue "${Esc}[34m")
set(ColourReset "${Esc}[m")
message(STATUS "${Red}Build mode: ${CMAKE_BUILD_TYPE} ${ColourReset}")

# Dependencies
add_subdirectory(${GLOBAL_DEPENDENCIES_PATH}/boost)

add_subdirectory(${GLOBAL_DEPENDENCIES_PATH}/mongo)

# DotCpp Api project
add_subdirectory(${RUNTIME_PATH}/cl/dotcpp/main)

# DotCpp Tests project
add_subdirectory(${RUNTIME_PATH}/cl/dotcpp/test)

# Runtime Main project
add_subdirectory(${RUNTIME_PATH}/cl/runtime/main)

# Runtime Console project
add_subdirectory(${RUNTIME_PATH}/cl/runtime/console)

# Runtime Tests project
add_subdirectory(${RUNTIME_PATH}/cl/runtime/test)
