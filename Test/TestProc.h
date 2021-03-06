#ifndef TESTPROC_H
#define TESTPROC_H

#include "TestSpell.h"

class EquipmentDb;
class Proc;

class TestProc : public TestSpell {
public:
    TestProc(EquipmentDb* equipment_db, QString proc_under_test);

protected:
    void then_proc_range_is(Proc* proc, const unsigned);
private:
};

#endif // TESTPROC_H
