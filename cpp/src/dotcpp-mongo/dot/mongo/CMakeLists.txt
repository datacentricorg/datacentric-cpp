cmake_minimum_required (VERSION 3.7.2)

set(CMAKE_CXX_FLAGS "-fPIC")

file(GLOB_RECURSE dotcpp_mongo_files "*.cpp" "*.hpp")

add_library(dotcpp_mongo ${dotcpp_mongo_files})

#copy executable to bin directory
add_custom_command(TARGET dotcpp_mongo
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dotcpp_mongo> "${PROJECT_SOURCE_DIR}/${BIN_FOLDER}")
