#ifndef ESSENCEOFTHERED_H
#define ESSENCEOFTHERED_H

#include "Buff.h"

class PeriodicResourceGainSpell;

class EssenceOfTheRed: public Buff {
public:
    EssenceOfTheRed(Character* pchar);
    ~EssenceOfTheRed() override;

    void buff_effect_when_applied() override;
    void buff_effect_when_removed() override;

private:
    PeriodicResourceGainSpell* spell;
    void prepare_set_of_combat_iterations_spell_specific() override;
};

#endif // ESSENCEOFTHERED_H
