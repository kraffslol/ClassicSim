#ifndef DEADLINESS_H
#define DEADLINESS_H

#include <QVector>

#include "Talent.h"

class TalentTree;

class Deadliness: public Talent {
public:
    Deadliness(Character* pchar, TalentTree* tree);

private:
    QVector<int> ap_multipliers;

    void apply_rank_effect() override;
    void remove_rank_effect() override;
};

#endif // DEADLINESS_H
