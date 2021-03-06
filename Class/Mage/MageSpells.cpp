
#include "MageSpells.h"
#include "Mage.h"

#include "MainhandAttack.h"


MageSpells::MageSpells(Mage* mage) :
    CharacterSpells(mage),
    mage(mage)
{
    this->mh_attack = new MainhandAttack(mage);

    spells.append(mh_attack);
}

MageSpells::~MageSpells() = default;
