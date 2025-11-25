cmake_minimum_required(VERSION 3.10)

project(JarPacker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        "src/main.cpp" "src/core/patcher_core.hpp" "src/patcher_core.cpp" "include/miniz.c"   "src/utils/string_utils.h" "src/utils/utils.h" "src/utils.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include)