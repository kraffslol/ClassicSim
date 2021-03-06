#ifndef BERSERKERSTANCEBUFF_H
#define BERSERKERSTANCEBUFF_H

#include "Buff.h"

class Warrior;

class BerserkerStanceBuff: public Buff {
public:
    BerserkerStanceBuff(Character* pchar);

    void buff_effect_when_applied() override;
    void buff_effect_when_removed() override;

private:
    Warrior* warr;
};

#endif // BERSERKERSTANCEBUFF_H
