#ifndef MELEESPECIALTABLE_H
#define MELEESPECIALTABLE_H

#include "PhysicalAttackResult.h"

class Random;

class MeleeSpecialTable {
public:
    MeleeSpecialTable(Random* _rand,
                      const int wpn_skill,
                      const unsigned miss_range,
                      const double dodge,
                      const double parry,
                      const double block);

    int get_outcome(const unsigned roll,
                    const unsigned crit_chance,
                    const bool include_dodge = true,
                    const bool include_parry = true,
                    const bool include_block = true,
                    const bool include_miss = true);

    void update_miss_chance(const unsigned miss_range);
    void update_dodge_chance(const double dodge);
    void update_parry_chance(const double parry);
    void update_block_chance(const double block);

    int get_wpn_skill() const;

private:
    Random* random;
    const int wpn_skill;

    unsigned miss_range;
    unsigned dodge_range;
    unsigned parry_range;
    unsigned block_range;
};

#endif // MELEESPECIALTABLE_H
