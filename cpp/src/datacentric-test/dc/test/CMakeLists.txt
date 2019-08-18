cmake_minimum_required(VERSION 3.7.2)

file(GLOB_RECURSE datacentric_test_files "*.cpp" "*.hpp")

SET(GCC_COVERAGE_LINK_FLAGS "-lresolv -lrt -pthread")


add_executable(datacentric_tests main.cpp ${datacentric_test_files})
target_link_libraries(datacentric_tests BoostLib)
target_link_libraries(datacentric_tests datacentric_api)
target_link_libraries(datacentric_tests MongoLib)
target_link_libraries(datacentric_tests dotcpp_api ${GCC_COVERAGE_LINK_FLAGS})
