/// @author    Chnossos
/// @date      Created on 2026-04-23

#pragma once

// Project includes
#include "../constants/ActorType.hpp"

#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <filesystem>
#include <unordered_map>

struct NpcInfo
{
    size_t id = 0;
    std::string name, title;
    u32 titleColor = 0xffffffff;
    ActorType type = ActorType::Npc;
    double baseSpeed = 100;
};

class NpcDirectory
{
    NpcDirectory() noexcept = default;

public:
    static void load(std::filesystem::path const & npcNamesFile, std::filesystem::path const & npcGroupsFile);

public:
    static auto npcCount()     -> size_t;
    static auto monsterCount() -> size_t;
    static auto totalCount()   -> size_t;

    static auto find   (size_t id) -> OptRef<NpcInfo>;
    static auto npc    (size_t id) -> OptRef<NpcInfo>;
    static auto monster(size_t id) -> OptRef<NpcInfo>;

private:
    static std::unordered_map<size_t, NpcInfo> _monsters;
    static std::unordered_map<size_t, NpcInfo> _npcs;
};
