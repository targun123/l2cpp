/// @author    Chnossos
/// @date      Created on 2026-05-06

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

class Character;
struct CharacterCreationParameters;

namespace Orm
{
    auto fetchCharacterPreviews(AccountId) -> std::vector<std::unique_ptr<Character>>;
    void createCharacter(AccountId, CharacterCreationParameters const &);
}
