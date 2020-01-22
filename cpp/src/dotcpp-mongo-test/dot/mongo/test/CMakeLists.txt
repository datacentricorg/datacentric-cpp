cmake_minimum_required(VERSION 3.7.2)

file(GLOB_RECURSE dotcpp_mongo_test_files "*.cpp" "*.hpp")

SET(GCC_COVERAGE_LINK_FLAGS "-lresolv -lrt -pthread")

add_executable(dotcpp_mongo_tests main.cpp ${dotcpp_mongo_test_files})
target_link_libraries(dotcpp_mongo_tests dotcpp_mongo)
target_link_libraries(dotcpp_mongo_tests dotcpp_api )
target_link_libraries(dotcpp_mongo_tests mongo_lib)
target_link_libraries(dotcpp_mongo_tests boost_lib ${GCC_COVERAGE_LINK_FLAGS})
