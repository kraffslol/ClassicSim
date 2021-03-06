#ifndef RETRIBUTION_H
#define RETRIBUTION_H

#include "TalentTree.h"

class Paladin;
class PaladinSpells;

class Retribution: public TalentTree {
public:
    Retribution(Paladin* paladin);
    ~Retribution() override = default;

    Talent* get_benediction();
    Talent* get_improved_judgement();
    Talent* get_improved_seal_of_the_crusader();
    Talent* get_conviction();
    Talent* get_seal_of_command();
    Talent* get_two_handed_weapon_specialization();
    Talent* get_sanctity_aura();
    Talent* get_vengeance();

private:
    Paladin* paladin;
    PaladinSpells* spells;
};

#endif // RETRIBUTION_H
