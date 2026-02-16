project(${CMAKE_PROJECT_NAME}-common)

add_library(${PROJECT_NAME} OBJECT)

find_package(Boost REQUIRED)
find_package(spdlog REQUIRED COMPONENTS header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost spdlog::spdlog_header_only)

target_sources(${PROJECT_NAME}
    PUBLIC
        Misc.cpp
        l2cpp/network/SocketListener.cpp
)
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET  HEADERS
    BASE_DIRS l2cpp
    FILES
        l2cpp/Misc.hpp
        l2cpp/Typedefs.hpp
        l2cpp/network/SocketListener.hpp
)
