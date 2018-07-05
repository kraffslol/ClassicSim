#ifndef EXECUTE_H
#define EXECUTE_H

#include "Spell.h"

class Warrior;

class Execute: public Spell {
public:
    Execute(Engine* engine, Character* pchar, CombatRoll* roll);

    void spell_effect() override;
    void increase_effect_via_talent() override;
    void decrease_effect_via_talent() override;

protected:
private:
    Warrior* pchar;
    QVector<QPair<int, int>> spell_ranks;
    QVector<int> talent_ranks;
    int initial_dmg;
    int dmg_per_rage_converted;

    bool is_ready_spell_specific() const override;
};

#endif // EXECUTE_H
