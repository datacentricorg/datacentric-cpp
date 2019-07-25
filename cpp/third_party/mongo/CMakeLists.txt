file(GLOB_RECURSE MONGO_BSON "../bson/*.c")
file(GLOB_RECURSE MONGO_COMMON "../common/*.c")
file(GLOB_RECURSE MONGO_JSONSL "../jsonsl/*.c")
file(GLOB_RECURSE MONGO_MONGOC "../mongoc/*.c")
file(GLOB_RECURSE MONGO_ZLIB "../zlib/*.c")
file(GLOB_RECURSE MONGO_BSONCXX "../bsoncxx/*.cpp" "../bsoncxx/**/*.cpp")
file(GLOB_RECURSE MONGO_MONGOCXX "../mongocxx/*.cpp" "../mongocxx/**/*.cpp")

set(CMAKE_CXX_FLAGS "-fPIC")

include_directories(.. ../mongoc ../bson ../common ../jsonsl ../zlib ../bsoncxx/third_party/mnmlstc ../bsoncxx) 
add_definitions(-DMONGOCXX_STATIC -DMONGO_CXX_DRIVER_COMPILING -D_ENABLE_EXTENDED_ALIGNED_STORAGE -DBSONCXX_STATIC -DBSON_STATIC -DMONGOC_STATIC -DMONGOC_COMPILATION -DBSON_COMPILATION -DBSON_STATIC -DJSONSL_PARSE_NAN)


add_library(MongoLib ${MONGO_BSON} ${MONGO_COMMON} ${MONGO_JSONSL} ${MONGO_MONGOC} ${MONGO_ZLIB} ${MONGO_BSONCXX} ${MONGO_MONGOCXX})


