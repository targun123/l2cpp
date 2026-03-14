/// @author    Chnossos
/// @date      Created on 2026-03-13

#include "SkillInfoRegister.hpp"

// Project includes
#include <l2cpp/Exception.hpp>

// Third-pary includes
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <fstream>

std::unordered_map<SkillUid, SkillInfo> SkillInfoRegister::_infos;

namespace
{
    template<typename T> requires std::integral<T> || std::floating_point<T>
    auto stringViewTo(std::string_view str) -> T
    {
        T value;
        auto const result = std::from_chars(str.data(), str.data() + str.size(), value);
        L2CPP_B_ASSERT(result.ec == std::errc(), "Failed to convert '{}' into a number", str);
        return value;
    }

    void loadSkillNames(std::unordered_map<SkillUid, SkillInfo> & infos, std::filesystem::path const & path)
    {
        std::ifstream file(path);
        L2CPP_B_ASSERT(file, "Failed to open file '{}': {}", path.string(), std::strerror(errno));

        std::string line;
        std::vector<std::string_view> parts;
        parts.reserve(6); // skill_id, skill_level, skill_name, skill_desc, skill_ench_route_name, skill_ench_desc

        for (size_t i = 1; std::getline(file, line) && !line.empty(); ++i) try
        {
            boost::algorithm::split(parts, line, boost::algorithm::is_any_of("\t"));
            if (parts.size() < 5 || parts.size() > 6)
                L2CPP_THROW("Found {} parts instead of minimum of 5:\nParts found: {::?}", parts.size(), parts);

            SkillInfo info(stringViewTo<u16>(parts[0]), std::string(parts[2].substr(2, parts[2].size() - 4)),
                           stringViewTo<u16>(parts[1]));
            auto const & [it, ok] = infos.try_emplace(info.uid(), std::move(info));
            L2CPP_B_ASSERT(ok, "Skill '{}' ({}) has already been loaded", it->second.fullName(), it->second.id());
        }
        catch (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Failed to load skill from '{}:{}':\n{}", path.string(), i, l2cpp::formatExceptionStack(e));
        }
    }

    void loadSkillGroups(std::unordered_map<SkillUid, SkillInfo> & infos, std::filesystem::path const & path)
    {
        std::ifstream file(path);
        L2CPP_B_ASSERT(file, "Failed to open file '{}': {}", path.string(), std::strerror(errno));

        std::string line;
        std::vector<std::string_view> parts;
        parts.reserve(17); /* skill_id, skill_level, oper_type, mp_consume, cast_range, cast_style, hit_time, is_magic,
                              ani_char, desc, icon_name, extra_eff, is_ench, ench_skill_id, hp_consume, UNK_0, UNK_1 */

        for (size_t i = 1; std::getline(file, line) && !line.empty(); ++i) try
        {
            boost::algorithm::split(parts, line, boost::algorithm::is_any_of("\t"));
            if (parts.size() != 17)
            {
                L2CPP_THROW("Found {} parts instead of 17:\nParts found: {::?}", parts.size(), parts);
            }

            auto const id  = stringViewTo<u16>(parts[0]);
            auto const lvl = stringViewTo<u16>(parts[1]);

            auto const it = infos.find({id, lvl});
            if (it == infos.end())
            {
                SPDLOG_WARN("Skill {} lv. {} has not been loaded", id, lvl);
                continue;
            }

            it->second.setCastDuration(MSec(static_cast<u16>(stringViewTo<double>(parts[6]) * 1000)));
        }
        catch  (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Failed to load skill from '{}:{}':\n{}", path.string(), i, l2cpp::formatExceptionStack(e));
        }
        // oper_type: 0=active_target 1=active_no_target 2=passive 3=toggle
        // cast_style: 0=instant/passive 1=cast_time_non_zero 2=magic_skills_only 3=physical_skills_only
        //             4=hp_drain_magic 5=bow_skill 6=seal_of_ruler 7=physical_projectile 8=double_attack
        //             9=dual_weapon(dual/fists) 10=force_discharge
    }
}

void SkillInfoRegister::load(std::filesystem::path const & skillNamesFile,
                             std::filesystem::path const & skillGroupsFile)
{

    loadSkillNames(_infos, skillNamesFile);
    loadSkillGroups(_infos, skillGroupsFile);
}

auto SkillInfoRegister::size() -> size_t
{
    return _infos.size();
}

auto SkillInfoRegister::skill(SkillUid const uid) -> OptionalRef<SkillInfo>
{
    OptionalRef<SkillInfo> skill;

    if (auto const it = _infos.find(uid); it != _infos.end())
        skill = it->second;

    return skill;
}
