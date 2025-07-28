cmake_minimum_required(VERSION 3.15)
project(CustomSTL LANGUAGES CXX)

# Uses C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory for your headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Executable for tests 
# add_executable(test_vector test/test_vector.cpp) (Uncomment when test_vector.cpp exists)

# Compiler flags: Enable warnings only when executable target exists
# target_compile_options(test_vector PRIVATE -Wall -Wextra) (Uncomment this after adding the executable above)