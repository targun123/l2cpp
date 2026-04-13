project(${CMAKE_PROJECT_NAME}-common)

add_library(${PROJECT_NAME} OBJECT)

find_package(Boost  REQUIRED)
find_package(spdlog REQUIRED COMPONENTS header_only)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::headers spdlog::spdlog_header_only)

target_sources(${PROJECT_NAME}
    PRIVATE
        l2cpp/Exception.cpp
        l2cpp/Misc.cpp
        l2cpp/network/Packet.cpp
        l2cpp/network/SocketListener.cpp
)
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET  HEADERS
    BASE_DIRS l2cpp
    FILES
        l2cpp/CompileTimeConfig.hpp
        l2cpp/Exception.hpp
        l2cpp/Misc.hpp
        l2cpp/OptRef.hpp
        l2cpp/Pimpl.hpp
        l2cpp/Typedefs.hpp
        l2cpp/utils/Enum.hpp
        l2cpp/utils/Traits.hpp
        l2cpp/network/Packet.hpp
        l2cpp/network/PacketReader.hpp
        l2cpp/network/Serialization.hpp
        l2cpp/network/SocketListener.hpp
)
