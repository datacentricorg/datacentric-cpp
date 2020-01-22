cmake_minimum_required (VERSION 3.7.2)
project(Datacentric)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fpermissive")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fpermissive")

#set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2")


add_definitions(-DBOOST_ALL_NO_LIB)


set(GLOBAL_DEPENDENCIES_PATH "${PROJECT_SOURCE_DIR}/third_party")
set(DOTCPP_PATH "${PROJECT_SOURCE_DIR}/src/dotcpp")
set(DOTCPP_MONGO_PATH "${PROJECT_SOURCE_DIR}/src/dotcpp-mongo")
set(DOTCPP_TEST_PATH "${PROJECT_SOURCE_DIR}/src/dotcpp-test")
set(DOTCPP_MONGO_TEST_PATH "${PROJECT_SOURCE_DIR}/src/dotcpp-mongo-test")
set(DATACENTRIC_PATH "${PROJECT_SOURCE_DIR}/src/datacentric")
set(DATACENTRIC_CLI_PATH "${PROJECT_SOURCE_DIR}/src/datacentric-cli")
set(DATACENTRIC_TEST_PATH "${PROJECT_SOURCE_DIR}/src/datacentric-test")


include_directories(${GLOBAL_DEPENDENCIES_PATH})
include_directories(${DOTCPP_PATH})
include_directories(${DOTCPP_MONGO_PATH})
include_directories(${DOTCPP_TEST_PATH})
include_directories(${DOTCPP_MONGO_TEST_PATH})
include_directories(${DATACENTRIC_PATH})
include_directories(${DATACENTRIC_CLI_PATH})
include_directories(${DATACENTRIC_TEST_PATH})

#directory for executables
file(MAKE_DIRECTORY "bin")
file(MAKE_DIRECTORY "bin/x64")
file(MAKE_DIRECTORY "bin/x64/Debug")
file(MAKE_DIRECTORY "bin/x64/Release")

#set appropriate folder for executables collection
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(BIN_FOLDER "bin/x64/Release")
	set(LIB_PATH "${GLOBAL_DEPENDENCIES_PATH}/lib/x64/Release")
else()
	set(CMAKE_BUILD_TYPE "Debug")
	set(BIN_FOLDER "bin/x64/Debug")
	set(LIB_PATH "${GLOBAL_DEPENDENCIES_PATH}/lib/x64/Debug")
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
add_subdirectory(${DOTCPP_PATH}/dot)

# DotCpp Mongo project
add_subdirectory(${DOTCPP_MONGO_PATH}/dot/mongo)

# DotCpp Tests project
add_subdirectory(${DOTCPP_TEST_PATH}/dot/test)

# DotCpp Mongo Tests project
add_subdirectory(${DOTCPP_MONGO_TEST_PATH}/dot/mongo/test)

# Datacentric Main project
add_subdirectory(${DATACENTRIC_PATH}/dc)

# Datacentric Console project
add_subdirectory(${DATACENTRIC_CLI_PATH}/dc/cli)

# Datacentric Tests project
add_subdirectory(${DATACENTRIC_TEST_PATH}/dc/test)
