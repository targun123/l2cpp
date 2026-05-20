/// @author    Chnossos
/// @date      Created on 2026-04-23

#pragma once

// Project includes
#include "../../Typedefs.hpp"
#include "../constants/ActorType.hpp"

// C++ includes
#include <filesystem>
#include <unordered_map>

struct NpcInfo
{
    u32 id = 0;
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
    static void loadHtmls();

public:
    static auto npcCount()     -> size_t;
    static auto monsterCount() -> size_t;
    static auto totalCount()   -> size_t;

    static auto find   (std::string_view   name) -> std::vector<Ref<NpcInfo const>>;
    static auto find   (std::wstring_view wName) -> std::vector<Ref<NpcInfo const>>;
    static auto find   (size_t id) -> OptRef<NpcInfo const>;
    static auto npc    (size_t id) -> OptRef<NpcInfo const>;
    static auto monster(size_t id) -> OptRef<NpcInfo const>;
    static auto html(size_t id, size_t page) -> std::wstring;

private:
    static constexpr auto makeKey(u64 id, u64 page) -> uint64_t
    {
        return (id << 32) | page;
    }

private:
    static std::unordered_map<size_t, NpcInfo> _monsters;
    static std::unordered_map<size_t, NpcInfo> _npcs;
    static std::unordered_map<u64, std::wstring> _htmls;
};
