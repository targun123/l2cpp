/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "../game/actions/SkillAction.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/SkillDirectory.hpp"
#include "../network/packets/server/ActionFailedPacket.hpp"
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(SkillUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 skillId, forceAttack;
    bool disallowMovement;
    reader >> skillId >> forceAttack >> disallowMovement;

    auto & c         = *player.currentCharacter();
    auto const skill = c.skills().skill(static_cast<SkillId>(skillId));
    L2CPP_B_ASSERT(skill, "Character does not possess skill id '{}'", skillId);

    // TODO: check that the skill can actually be casted here (enough MP, relevant target if required, etc.)

    auto const target = c.target();

    // must have a target or not require one
    bool canCast = target.has_value() || skill->tmplate().targetType() == SkillTargetType::None;

    if (canCast && skill->tmplate().targetType() != SkillTargetType::None)
    {
        switch (skill->tmplate().targetNature())
        {
            case SkillTargetNature::None: break;
            case SkillTargetNature::Any:  break;

            case SkillTargetNature::Self:
                canCast = *target == c;
                break;

            case SkillTargetNature::Other:
                canCast = *target != c;
                break;

            case SkillTargetNature::Friendly:
                canCast = target->type() != ActorType::Monster;
                break;

            case SkillTargetNature::Ennemy:
                canCast = target->type() == ActorType::Monster || forceAttack;
                break;

            case SkillTargetNature::Monster:
                canCast = target->type() == ActorType::Monster || forceAttack;
                break;

            case SkillTargetNature::Character:
                canCast = target->type() == ActorType::Character;
                break;

            case SkillTargetNature::PartyMember:
                canCast = target->type() == ActorType::Character;
                break;

            case SkillTargetNature::ClanMember:
                canCast = target->type() == ActorType::Character;
                break;

            case SkillTargetNature::Corpse:
                canCast = !target->isAlive();
                break;

            case SkillTargetNature::Undead:
                canCast = target.has_value();
                break;

            case SkillTargetNature::Summon:
                canCast = target.has_value();
                break;

            case SkillTargetNature::Pet:
                canCast = target.has_value();
                break;

            case SkillTargetNature::Servitor:
                canCast = target.has_value();
                break;

            case SkillTargetNature::SummonOwner:
                canCast = target.has_value();
                break;

            case SkillTargetNature::Unlockable:
                canCast = target.has_value();
                break;
        }
    }

    if (canCast)
        c.doNext<SkillAction>(skill->tmplate());
    else
        player.connection().send(ActionFailedPacket{});
}
