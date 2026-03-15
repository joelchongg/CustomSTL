cmake_minimum_required(VERSION 3.15)
project(CustomSTL LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add GoogleTest
add_subdirectory(googletest)
include(GoogleTest)

# Automatically pick up any test source matching test/test_*.cpp.
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/test_*.cpp)

# Enable CTest testing
enable_testing()

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

    if (WIN32)
        target_link_libraries(${TEST_NAME} PRIVATE gtest_main)
    else()
        target_link_libraries(${TEST_NAME} PRIVATE gtest_main pthread)
    endif()

    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)
    endif()

    gtest_discover_tests(${TEST_NAME})
endforeach()