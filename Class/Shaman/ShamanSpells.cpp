
#include "ShamanSpells.h"
#include "Shaman.h"

#include "MainhandAttack.h"


ShamanSpells::ShamanSpells(Shaman* shaman) :
    CharacterSpells(shaman),
    shaman(shaman)
{
    this->mh_attack = new MainhandAttack(shaman);

    spells.append(mh_attack);
}

ShamanSpells::~ShamanSpells() = default;
