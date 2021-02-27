cmake_minimum_required(VERSION 3.20)

add_executable(demo examples/example.cpp)
target_include_directories(demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Boost 1.75 REQUIRED)

if (${Boost_FOUND})
    target_include_directories(demo PUBLIC ${Boost_INCLUDE_DIRS})
endif ()
