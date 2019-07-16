cmake_minimum_required(VERSION 3.7.2)

file(GLOB_RECURSE runtime_test_files "*.cpp" "*.hpp")

SET(GCC_COVERAGE_LINK_FLAGS "-lresolv -lrt -pthread")


add_executable(runtime_tests main.cpp ${runtime_test_files})
target_link_libraries(runtime_tests BoostLib)
target_link_libraries(runtime_tests runtime_api)
target_link_libraries(runtime_tests MongoLib)
target_link_libraries(runtime_tests dotcpp_api ${GCC_COVERAGE_LINK_FLAGS})

