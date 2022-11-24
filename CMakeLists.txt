cmake_minimum_required(VERSION 3.21)
project(search_engine)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

add_executable(search_engine main.cpp ConverterJSON.cpp InvertedIndex.cpp SearchServer.cpp )


target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
