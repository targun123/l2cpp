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
        Player.cpp
        Player.hpp
        game/Character.cpp
        game/Character.hpp
        game/Shortcut.cpp
        game/Shortcut.hpp
        handlers/_Common.hpp
        handlers/Authenticate.cpp
        handlers/CharacterCancelDeletion.cpp
        handlers/CharacterCreate.cpp
        handlers/CharacterList.cpp
        handlers/CharacterSelect.cpp
        handlers/CharacterShowCreationScreen.cpp
        handlers/ConnectionClosing.cpp
        handlers/EnterWorld.cpp
        handlers/LeaveWorld.cpp
        handlers/ManorList.cpp
        handlers/MiniMapShow.cpp
        handlers/Move.cpp
        handlers/MoveUpdate.cpp
        handlers/PacketHandlers.cpp
        handlers/PacketHandlers.hpp
        handlers/Protocol.cpp
        handlers/QuestList.cpp
        handlers/ShortcutBarAdd.cpp
        handlers/ShortcutBarRemove.cpp
        handlers/SkillList.cpp
        handlers/SkillUse.cpp
        handlers/TargetClear.cpp
        handlers/TargetSelect.cpp
        network/Connection.cpp
        network/Connection.hpp
        network/packets/client/OpCodes.hpp
        network/packets/server/ProtocolHandshakePacket.cpp
        network/packets/server/ProtocolHandshakePacket.hpp
        network/packets/server/CharacterStatusUpdatePacket.cpp
        network/packets/server/CharacterStatusUpdatePacket.hpp
        network/packets/server/EntityStatusUpdatePacket.cpp
        network/packets/server/EntityStatusUpdatePacket.hpp
)
