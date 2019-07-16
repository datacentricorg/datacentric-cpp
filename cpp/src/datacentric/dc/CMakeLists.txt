cmake_minimum_required (VERSION 3.7.2)

set(CMAKE_CXX_FLAGS "-fPIC")

file(GLOB_RECURSE runtime_api_files "*.cpp" "*.hpp")

add_library(runtime_api ${runtime_api_files})

#copy executable to bin directory
add_custom_command(TARGET runtime_api 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:runtime_api> "${PROJECT_SOURCE_DIR}/${BIN_FOLDER}")
