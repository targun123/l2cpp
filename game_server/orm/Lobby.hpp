/// @author    Chnossos
/// @date      Created on 2026-05-06

#pragma once

// Project includes
#include "../Typedefs.hpp"

class Character;

namespace Orm
{
    auto fetchCharacterPreviews(AccountId) -> std::vector<Ref<Character>>;
    void createCharacter(AccountId, Character &);
    void selectCharacter(AccountId, std::wstring_view selectedCharName);
    void loadGear(Character & c);
}
