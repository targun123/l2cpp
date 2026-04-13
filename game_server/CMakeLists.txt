project(game_server)

add_executable(${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<BOOL:WIN32>:SPDLOG_WCHAR_TO_UTF8_SUPPORT>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_PROJECT_NAME}-common)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")
target_sources(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-common> ${SOURCES} ${HEADERS})
