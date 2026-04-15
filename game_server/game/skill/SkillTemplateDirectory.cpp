/// @author    Chnossos
/// @date      Created on 2026-03-13

#include "SkillTemplateDirectory.hpp"

// Project includes
#include "../../utils/Conversion.hpp"

#include <l2cpp/Exception.hpp>

// Third-pary includes
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <fstream>

std::unordered_map<SkillUid, SkillTemplate> SkillTemplateDirectory::_templates;

namespace
{
    void loadSkillNames(std::unordered_map<SkillUid, SkillTemplate> & infos, std::filesystem::path const & path)
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

            SkillTemplate tmpl(Utils::stringViewTo<u16>(parts[0]), std::string(parts[2].substr(2, parts[2].size() - 4)),
                               Utils::stringViewTo<u16>(parts[1]));
            auto const & [it, ok] = infos.try_emplace(tmpl.uid(), std::move(tmpl));
            L2CPP_B_ASSERT(ok, "Skill '{}' ({}) has already been loaded", it->second.fullName(), it->second.id());
        }
        catch (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Failed to load skill from '{}:{}':\n{}", path.string(), i, l2cpp::formatExceptionStack(e));
        }
    }

    void loadSkillGroups(std::unordered_map<SkillUid, SkillTemplate> & infos, std::filesystem::path const & path)
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
            L2CPP_B_ASSERT(parts.size() == 17,
                           "Found {} parts instead of 17:\nParts found: {::?}", parts.size(), parts);

            auto const id  = Utils::stringViewTo<u16>(parts[0]);
            auto const lvl = Utils::stringViewTo<u16>(parts[1]);

            auto const it = infos.find({id, lvl});
            if (it == infos.end())
            {
                SPDLOG_WARN("Skill {} lv. {} has not been loaded", id, lvl);
                continue;
            }

            auto & skill = it->second;

            std::chrono::duration<double> const duration{Utils::stringViewTo<double>(parts[6])};
            skill.setCastDuration(std::chrono::floor<std::chrono::milliseconds>(duration));

            enum class SkillOperateType : u8 { ActiveTarget, ActiveNoTarget, Passive, Toggle };
            switch (static_cast<SkillOperateType>(Utils::stringViewTo<u8>(parts[2])))
            {
                case SkillOperateType::ActiveTarget:
                case SkillOperateType::ActiveNoTarget: skill.setType(SkillType::Active);  break;
                case SkillOperateType::Passive:        skill.setType(SkillType::Passive); break;
                case SkillOperateType::Toggle:         skill.setType(SkillType::Toggle);  break;
            }
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

void SkillTemplateDirectory::load(std::filesystem::path const & skillNamesFile,
                                  std::filesystem::path const & skillGroupsFile)
{
    loadSkillNames(_templates, skillNamesFile);
    loadSkillGroups(_templates, skillGroupsFile);
}

auto SkillTemplateDirectory::size() -> size_t { return _templates.size(); }

auto SkillTemplateDirectory::skill(SkillUid const uid) -> OptRef<SkillTemplate>
{
    auto const it = _templates.find(uid);
    return it != _templates.end() ? OptRef(it->second) : std::nullopt;
}
