cmake_minimum_required(VERSION 3.13)

project(smileTest)

set(CMAKE_CXX_STANDARD 17)

message(${CMAKE_CXX_COMPILER_VERSION})
message(${CMAKE_CXX_COMPILER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/boost/)

add_executable(smileTest
        main.cpp
        networkLoader.h
        lib/tinyxml2/tinyxml2.h
        lib/tinyxml2/tinyxml2.cpp
        )

add_library(smile STATIC IMPORTED functional_helpers.hpp)
target_link_directories(smileTest PUBLIC ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib)
target_link_libraries(smileTest ${CMAKE_SOURCE_DIR}/lib/smile/libsmile.a)
