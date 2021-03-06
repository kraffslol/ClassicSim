#ifndef SANCTITYAURABUFF_H
#define SANCTITYAURABUFF_H

#include "Buff.h"

class Paladin;

class SanctityAuraBuff: public Buff {
public:
    SanctityAuraBuff(Paladin* pchar);
    ~SanctityAuraBuff() override = default;

private:
    Paladin* paladin;

    void buff_effect_when_applied() override;
    void buff_effect_when_removed() override;
};

#endif // SANCTITYAURABUFF_H
