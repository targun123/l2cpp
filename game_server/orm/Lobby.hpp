/// @author    Chnossos
/// @date      Created on 2026-05-06

#pragma once

// Project includes
#include "../Typedefs.hpp"

class Character;

namespace Orm
{
    auto fetchCharacterPreviews(AccountId) -> std::vector<std::unique_ptr<Character>>;
    void createCharacter(AccountId, Character const &);
    void selectCharacter(AccountId, std::wstring_view selectedCharName);
}
