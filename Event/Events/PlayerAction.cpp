#include "PlayerAction.h"

#include "CharacterSpells.h"

PlayerAction::PlayerAction(CharacterSpells* pchar, const double timestamp):
    Event(Events::PlayerAction)
{
    this->spells = pchar;
    this->priority = timestamp;
    this->name = "PlayerAction";
}

void PlayerAction::act() {
    spells->perform_rotation();
}
