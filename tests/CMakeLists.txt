cmake_minimum_required(VERSION 3.13)
project(test_tagsystem)
set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtest REQUIRED gtest>=1.8.1)

SET(CMAKE_CXX_FLAGS -pthread)
enable_testing()

include_directories(${gtest_INCLUDE_DIRS})

add_executable(test_tagsystem
    main.cpp
    test_tag.cpp)

target_link_libraries(test_tagsystem ${gtest_LIBRARIES})
