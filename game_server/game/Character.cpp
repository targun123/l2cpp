/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include "inventory/Gear.hpp"
#include "inventory/ItemStorage.hpp"
#include "skill/SkillTemplateDirectory.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <array>
#include <ranges>

struct Character::CharacterImpl
{
    Gear                      gear;
    ItemStorage               inventory;
    SkillRegistry             skills;
    std::array<Shortcut, 120> shortcuts{};

    auto skill(SkillId id) const -> OptionalRef<Skill const>;
};

template class Pimpl<Character::CharacterImpl>;

auto Character::CharacterImpl::skill(SkillId id) const -> OptionalRef<Skill const>
{
    OptionalRef<Skill const> skill;

    auto const it = std::ranges::find_if(skills, [id] (auto const & p) { return p.first.id() == id; });
    if (it != skills.end())
        skill.emplace(it->second);

    return skill;
}

// ---------------------------------------------------------------------------------------------------------------------

Character::Character()
{
    Item formalWear;
    formalWear.tmplate.id       = 6408;
    formalWear.tmplate.name     = "formalWear";
    formalWear.tmplate.category = ItemCategory::Armor;
    formalWear.tmplate.bodyPart = GearSlot::Dress;
    Ref item = _impl->inventory.add(std::move(formalWear));
    _impl->gear.equipItem(item.get());

    Item infinitySpear;
    infinitySpear.tmplate.id       = 6621;
    infinitySpear.tmplate.name     = "infinitySpear";
    infinitySpear.tmplate.category = ItemCategory::Weapon;
    infinitySpear.tmplate.bodyPart = GearSlot::Hands;
    item = _impl->inventory.add(std::move(infinitySpear));
     _impl->gear.equipItem(item.get());

    Item infinityBlade;
    infinityBlade.tmplate.id       = 6611;
    infinityBlade.tmplate.name     = "infinityBlade";
    infinityBlade.tmplate.category = ItemCategory::Weapon;
    infinityBlade.tmplate.bodyPart = GearSlot::RightHand;
    item = _impl->inventory.add(std::move(infinityBlade));
    _impl->gear.equipItem(item.get());

    Item imperialCrusaderShield;
    imperialCrusaderShield.tmplate.id       = 6377;
    imperialCrusaderShield.tmplate.name     = "imperialCrusaderShield";
    imperialCrusaderShield.tmplate.category = ItemCategory::Armor;
    imperialCrusaderShield.tmplate.bodyPart = GearSlot::LeftHand;
    item = _impl->inventory.add(std::move(imperialCrusaderShield));
    _impl->gear.equipItem(item.get());

    if (accessLevel > 0)
    {
        auto const superHaste = SkillTemplateDirectory::skill(7029, 4);
        _impl->skills.try_emplace(superHaste->get().uid(), superHaste->get());
    }

    auto const hurricane = SkillTemplateDirectory::skill(1239, 1);
    _impl->skills.try_emplace(hurricane->get().uid(), hurricane->get());
}

Character::~Character() = default;

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;

auto Character::inventory()       -> ItemStorage       & { return _impl->inventory; }
auto Character::inventory() const -> ItemStorage const & { return _impl->inventory; }

auto Character::gear()       -> Gear       & { return _impl->gear; }
auto Character::gear() const -> Gear const & { return _impl->gear; }

auto Character::skills() const -> SkillRegistry const &
{
    return _impl->skills;
}

auto Character::setShortcut(Shortcut shortcut) -> Shortcut &
{
    L2CPP_B_ASSERT(shortcut.index(), "Cannot set a shortcut whose index ({}) is invalid", *shortcut.index());
    auto const index = *shortcut.index();

    if (shortcut.type() == Shortcut::Type::Skill)
    {
        if (auto const skill = _impl->skill(static_cast<SkillId>(shortcut.targetId())); skill)
            shortcut.setSkillLevel(skill->get().tmplate().level());
    }

    _impl->shortcuts[index] = std::move(shortcut);
    return _impl->shortcuts[index];
}

void Character::delShortcut(size_t const index)
{
    L2CPP_B_ASSERT(index < _impl->shortcuts.size(), "Cannot remove a shortcut whose index ({}) is invalid", index);
    _impl->shortcuts[index] = Shortcut();
}
