#ifndef BERSERKING_H
#define BERSERKING_H

#include "Spell.h"

class BerserkingBuff;

class Berserking: public Spell {
public:
    Berserking(Character* pchar);
    ~Berserking() override;

private:
    BerserkingBuff* buff;

    void spell_effect() override;
    void enable_spell_effect() override;
    void disable_spell_effect() override;
};

#endif // BERSERKING_H
