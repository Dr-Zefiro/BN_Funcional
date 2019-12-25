cmake_minimum_required(VERSION 3.15)

project(smileTest)

set(CMAKE_CXX_STANDARD 20)

message(${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION})

set(CMAKE_C_FLAGS_RELEASE   "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(globalPerfFlags "-march=native -fprofile-correction -fprofile-generate=./")
#set(globalPerfFlags "-march=native -ffast-math -fprofile-correction -fprofile-use=./")
set(globalPerfFlags "-march=native -ffast-math -fno-exceptions")
set(globalWarningFlags "-Wall -Wextra -fno-omit-frame-pointer")

set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} ${globalPerfFlags} ${globalWarningFlags}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${globalPerfFlags} ${globalWarningFlags}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/boost/)

add_executable(smileTest
        main.cpp
        networkLoader.h
        MCIntegrator.h
        lib/tinyxml2/tinyxml2.h
        lib/tinyxml2/tinyxml2.cpp
    )

add_library(smile STATIC IMPORTED functional_helpers.hpp)
target_link_directories(smileTest PUBLIC ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/smile/libsmile.a)
target_link_libraries(${PROJECT_NAME} pthread stdc++)

