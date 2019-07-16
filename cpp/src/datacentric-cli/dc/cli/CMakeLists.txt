cmake_minimum_required(VERSION 3.7.2)

file(GLOB_RECURSE runtime_console_files "*.cpp" "*.hpp")

add_executable(runtime_console main.cpp ${runtime_console_files})
target_link_libraries(runtime_console BoostLib)
target_link_libraries(runtime_console runtime_api)
