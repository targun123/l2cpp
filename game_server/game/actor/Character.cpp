/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include "../Shortcut.hpp"
#include "../components/PlayerAppearance.hpp"
#include "../inventory/Gear.hpp"
#include "../inventory/ItemStorage.hpp"
#include "../skill/SkillDirectory.hpp"
#include "../skill/SkillTemplateDirectory.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/utils/Enum.hpp>

// C++ includes
#include <array>

struct Character::CharacterImpl
{
    Gear                      gear;
    ItemStorage               inventory;
    SkillDirectory            skills;
    std::array<Shortcut, 120> shortcuts{};
};

template class Pimpl<Character::CharacterImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Character::Character()
{
    setPosition(-83968, 244634, -3500); // Talking Island GK
    auto & appearance = component<PlayerAppearance>();
    appearance.collisionHeight = 23.5; // Male human
    appearance.collisionRadius = 9;    // Male human

    addComponent<PlayerAppearance>();

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
        _impl->skills.learn(7029, 4); // Super Haste

    _impl->skills.learn(1239, 1); // Hurricane
}

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;
Character::~Character() = default;

auto Character::appearance()       -> PlayerAppearance       & { return component<PlayerAppearance>(); }
auto Character::appearance() const -> PlayerAppearance const & { return component<PlayerAppearance>(); }

auto Character::inventory()       -> ItemStorage       & { return _impl->inventory; }
auto Character::inventory() const -> ItemStorage const & { return _impl->inventory; }

auto Character::gear()       -> Gear       & { return _impl->gear; }
auto Character::gear() const -> Gear const & { return _impl->gear; }

auto Character::skills() -> SkillDirectory &
{
    return _impl->skills;
}

auto Character::skills() const -> SkillDirectory const &
{
    return _impl->skills;
}

auto Character::setShortcut(Shortcut shortcut) -> Shortcut &
{
    L2CPP_B_ASSERT(shortcut.index(), "Cannot set a shortcut whose index ({}) is invalid", *shortcut.index());
    auto const index = *shortcut.index();

    if (shortcut.type() == ShortcutType::Skill)
    {
        if (auto const skill = _impl->skills.skill(static_cast<SkillId>(shortcut.targetId())); skill)
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
