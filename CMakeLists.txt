cmake_minimum_required(VERSION 4.2)
project(l2cpp)

set(CMAKE_CXX_STANDARD   23)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601)
    add_compile_options(/W4)
endif()

add_subdirectory(login_server)
