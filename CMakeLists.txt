cmake_minimum_required(VERSION 3.15)
project(CustomSTL LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add GoogleTest
add_subdirectory(googletest)

# Add executable target for various tests
add_executable(test_unique_ptr test/test_unique_ptr.cpp)

# Include directory for headers
target_include_directories(test_unique_ptr PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link GoogleTest and pthread
if (WIN32)
    target_link_libraries(test_unique_ptr gtest_main)
else()
    target_link_libraries(test_unique_ptr gtest_main pthread)
endif()

# Set warning flags per compiler
if(MSVC)
    target_compile_options(test_unique_ptr PRIVATE /W4 /WX)  # /WX treats warnings as errors
else()
    target_compile_options(test_unique_ptr PRIVATE -Wall -Wextra -Werror)
endif()

# Enable CTest testing
enable_testing()
add_test(NAME UniquePtrTests COMMAND test_unique_ptr)