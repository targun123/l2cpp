project(${CMAKE_PROJECT_NAME}-common)

add_library(${PROJECT_NAME} OBJECT)

find_package(fmt REQUIRED COMPONENTS header_only)
target_include_directories(${PROJECT_NAME} PRIVATE ${fmt_INCLUDE_DIRS})

target_sources(${PROJECT_NAME}
    PUBLIC
        Misc.cpp
)
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET  HEADERS
    BASE_DIRS l2cpp
    FILES
        l2cpp/Misc.hpp
        l2cpp/Typedefs.hpp
)
