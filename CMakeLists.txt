cmake_minimum_required(VERSION 3.15)
project(CustomSTL LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory for headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add executable target for your unique_ptr test
add_executable(test_unique_ptr test/test_unique_ptr.cpp)

# Set warning flags per compiler
if(MSVC)
    target_compile_options(test_unique_ptr PRIVATE /W4 /WX)  # /WX treats warnings as errors
else()
    target_compile_options(test_unique_ptr PRIVATE -Wall -Wextra -Werror)
endif()