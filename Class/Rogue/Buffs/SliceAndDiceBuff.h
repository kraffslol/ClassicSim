#ifndef SLICEANDDICEBUFF_H
#define SLICEANDDICEBUFF_H

#include <QVector>

#include "Buff.h"


class SliceAndDiceBuff: public Buff {
public:
    SliceAndDiceBuff(Character* pchar);

    void update_duration(const unsigned combo_points);
    void change_duration_modifier(const int);

private:
    QVector<int> durations;
    QVector<double> imp_snd_duration_modifiers;
    double imp_snd_modifier;


    void buff_effect_when_applied() override;
    void buff_effect_when_removed() override;
};

#endif // SLICEANDDICEBUFF_H
