cmake_minimum_required(VERSION 3.10)

project(JarPacker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /DNDEBUG /Zi")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Ob1 /DNDEBUG")
    
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    
    add_compile_options("$<$<CONFIG:RELEASE>:/MT>")
    add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:/MT>")
    add_compile_options("$<$<CONFIG:MINSIZEREL>:/MT>")
    add_compile_options("$<$<CONFIG:DEBUG>:/MTd>")
endif()

add_executable(${PROJECT_NAME}
    "src/main.cpp" 
    "src/core/patcher_core.hpp" 
    "src/patcher_core.cpp" 
    "include/miniz.c"   
    "src/utils/string_utils.h" 
    "src/utils/utils.h" 
    "src/utils.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB:msvcrt.lib")
    target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB:libcmt.lib")
endif()