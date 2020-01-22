cmake_minimum_required(VERSION 3.7.2)

file(GLOB_RECURSE datacentric_console_files "*.cpp" "*.hpp")

add_executable(datacentric_console main.cpp ${datacentric_console_files})
target_link_libraries(datacentric_console boost_lib)
target_link_libraries(datacentric_console datacentric_api)
