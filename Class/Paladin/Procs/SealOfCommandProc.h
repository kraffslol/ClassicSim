#ifndef SEALOFCOMMANDPROC_H
#define SEALOFCOMMANDPROC_H

#include "ProcPPM.h"

class Paladin;
class SealOfCommand;

class SealOfCommandProc: public ProcPPM {
public:
    SealOfCommandProc(Character* pchar, SealOfCommand* seal);

    void proc_effect() override;

private:
    SealOfCommand* seal;

    bool proc_specific_conditions_fulfilled() const override;
};

#endif // SEALOFCOMMANDPROC_H
