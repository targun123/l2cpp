project(${CMAKE_PROJECT_NAME}-game_server)

add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME game_server)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<BOOL:WIN32>:SPDLOG_WCHAR_TO_UTF8_SUPPORT>)

find_package(Boost   REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(spdlog  REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Boost::boost
        OpenSSL::Crypto
        spdlog::spdlog_header_only
)

target_sources(${PROJECT_NAME}
    PRIVATE
        $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-common>
    PRIVATE
        main.cpp
)
