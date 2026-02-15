cmake_minimum_required(VERSION 4.2)
cmake_policy(SET CMP0111 OLD)
project(l2cpp)

set(CMAKE_CXX_STANDARD   23)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601 SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
    add_compile_options(/Od /Z7 /W4 /diagnostics:column -FC -Gm- -GR-)
endif()

add_subdirectory(login_server)
