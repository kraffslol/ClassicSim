#ifndef TESTBUFF_H
#define TESTBUFF_H

#include "TestSpell.h"

class Buff;
class SimSettings;

class TestBuff : public TestSpell {
public:
    TestBuff(EquipmentDb* equipment_db, QString buff_under_test);

    void then_number_of_charges_is(Buff* buff, const int charges);

protected:
};

#endif // TESTBUFF_H
