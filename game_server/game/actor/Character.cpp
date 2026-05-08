/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include "../../network/packets/server/ui/UiConfirmationModalShowPacket.hpp"
#include "../World.hpp"
#include "../components/CharacterStatus.hpp"
#include "../components/Gear.hpp"
#include "../components/PlayerAppearance.hpp"
#include "../components/SkillDirectory.hpp"
#include "../inventory/ItemStorage.hpp"
#include "../skill/SkillTemplateDirectory.hpp"
#include "../ui/Shortcut.hpp"

#include <l2cpp/Exception.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <array>

enum class ConfirmationModalSystemMessageId : u32
{
    ResurrectionProposal = 1510,
};

struct Character::CharacterImpl
{
    Profession profession = Profession::HumanFighter;

    ItemStorage               inventory;
    std::array<Shortcut, 120> shortcuts{};

    std::optional<ConfirmationModalSystemMessageId> confirmationModalMessageId;
};

template class Pimpl<Character::CharacterImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Character::Character(OptRef<Player> p)
    : Actor(ActorType::Character)
    , player(std::move(p))
{
    auto & appearance = addComponent<PlayerAppearance>();
    appearance.collisionHeight = 23;
    appearance.collisionRadius = 9;

    addComponent<CharacterStatus>();

    if (accessLevel > 0)
        skills().learn(7029, 4); // Super Haste

    auto & gear = *component<Gear>();

    Item formalWear;
    formalWear.tmplate.id       = 6408;
    formalWear.tmplate.name     = "formalWear";
    formalWear.tmplate.category = ItemCategory::Armor;
    formalWear.tmplate.gearSlot = GearSlot::Dress;
    Ref item = _impl->inventory.add(std::move(formalWear));
    gear.equipItem(item.get());

    Item infinitySpear;
    infinitySpear.tmplate.id       = 6621;
    infinitySpear.tmplate.name     = "infinitySpear";
    infinitySpear.tmplate.category = ItemCategory::Weapon;
    infinitySpear.tmplate.grade    = ItemGrade::S;
    infinitySpear.tmplate.gearSlot = GearSlot::Hands;
    item = _impl->inventory.add(std::move(infinitySpear));
    gear.equipItem(item.get());

    Item infinityBlade;
    infinityBlade.tmplate.id       = 6611;
    infinityBlade.tmplate.name     = "infinityBlade";
    infinityBlade.tmplate.category = ItemCategory::Weapon;
    infinityBlade.tmplate.grade    = ItemGrade::S;
    infinityBlade.tmplate.gearSlot = GearSlot::RightHand;
    item = _impl->inventory.add(std::move(infinityBlade));
    gear.equipItem(item.get());

    Item imperialCrusaderShield;
    imperialCrusaderShield.tmplate.id       = 6377;
    imperialCrusaderShield.tmplate.name     = "imperialCrusaderShield";
    imperialCrusaderShield.tmplate.category = ItemCategory::Armor;
    imperialCrusaderShield.tmplate.grade    = ItemGrade::S;
    imperialCrusaderShield.tmplate.gearSlot = GearSlot::LeftHand;
    item = _impl->inventory.add(std::move(imperialCrusaderShield));
    gear.equipItem(item.get());

    Item infinityFang;
    infinityFang.tmplate.id       = 6618;
    infinityFang.tmplate.name     = "infinityFang";
    infinityFang.tmplate.category = ItemCategory::Weapon;
    infinityFang.tmplate.grade    = ItemGrade::S;
    infinityFang.tmplate.gearSlot = GearSlot::Hands;
    item = _impl->inventory.add(std::move(infinityFang));
    gear.equipItem(item.get());

    Item infinityBow;
    infinityBow.tmplate.id       = 6619;
    infinityBow.tmplate.name     = "infinityBow";
    infinityBow.tmplate.category = ItemCategory::Weapon;
    infinityBow.tmplate.grade    = ItemGrade::S;
    infinityBow.tmplate.gearSlot = GearSlot::Hands;
    item = _impl->inventory.add(std::move(infinityBow));
    gear.equipItem(item.get());

    Item sGradeArrows;
    sGradeArrows.tmplate.id = 1345;
    sGradeArrows.tmplate.name     = "sGradeArrows";
    sGradeArrows.tmplate.category = ItemCategory::Misc;
    sGradeArrows.tmplate.grade    = ItemGrade::S;
    sGradeArrows.tmplate.gearSlot = GearSlot::LeftHand;
    sGradeArrows.quantity         = 100;
    item = _impl->inventory.add(std::move(sGradeArrows));
    gear.equipItem(item.get());
}

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;
Character::~Character() = default;

auto Character::profession() const -> Profession { return _impl->profession; }

auto Character::status()       -> CharacterStatus       & { return component<CharacterStatus>(); }
auto Character::status() const -> CharacterStatus const & { return component<CharacterStatus>(); }

auto Character::appearance()       -> PlayerAppearance       & { return component<PlayerAppearance>(); }
auto Character::appearance() const -> PlayerAppearance const & { return component<PlayerAppearance>(); }

auto Character::inventory()       -> ItemStorage       & { return _impl->inventory; }
auto Character::inventory() const -> ItemStorage const & { return _impl->inventory; }

bool Character::isAttackable() const { return false; }

void Character::setProfession(Profession const profession) { _impl->profession = profession; }

auto Character::setShortcut(Shortcut shortcut) -> Shortcut &
{
    L2CPP_B_ASSERT(shortcut.index(), "Cannot set a shortcut whose index ({}) is invalid", *shortcut.index());
    auto const index = *shortcut.index();

    if (shortcut.type() == ShortcutType::Skill)
    {
        if (auto const skill = skills().skill(static_cast<SkillId>(shortcut.targetId())); skill)
            shortcut.setSkillLevel(skill->tmplate().level());
    }

    _impl->shortcuts[index] = std::move(shortcut);
    return _impl->shortcuts[index];
}

void Character::delShortcut(size_t const index)
{
    L2CPP_B_ASSERT(index < _impl->shortcuts.size(), "Cannot remove a shortcut whose index ({}) is invalid", index);
    _impl->shortcuts[index] = Shortcut();
}

void Character::offerResurrection(Actor const & emitter)
{
    if (isAlive())
        return;

    if (!player)
        return revive();

    using enum ConfirmationModalSystemMessageId;

    Network::Packet::Server::UiConfirmationModalShowPacket p(std::to_underlying(ResurrectionProposal));
    p << SysMsgArg::Text(emitter.name());
    p << SysMsgArg::Text(name());

    World::send(*this, std::move(p));

    _impl->confirmationModalMessageId = ResurrectionProposal;
}

void Character::answerConfirmationModal(u32 const systemMessageId, bool const accepted)
{
    L2CPP_B_ASSERT(_impl->confirmationModalMessageId, "[{}] wasn't expecting any modal answer", id());
    L2CPP_B_ASSERT(std::to_underlying(*_impl->confirmationModalMessageId) == systemMessageId,
                   "[{}] wasn't expecting a confirmation modal answer to system message '{}' (expected id: {})",
                   id(), systemMessageId, std::to_underlying(*_impl->confirmationModalMessageId));

    switch (*_impl->confirmationModalMessageId)
    {
        using enum ConfirmationModalSystemMessageId;

        case ResurrectionProposal:
        {
            if (accepted)
                revive();

            break;
        }

        default:
            L2CPP_THROW("[{}] Unhandled confirmation modal answer for message id '{}'",
                        id(), std::to_underlying(*_impl->confirmationModalMessageId));
    }
}
