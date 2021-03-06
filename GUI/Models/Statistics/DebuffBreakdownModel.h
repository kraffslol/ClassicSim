#ifndef DEBUFFBREAKDOWNMODEL_H
#define DEBUFFBREAKDOWNMODEL_H

#include <QAbstractListModel>
#include <QVector>

#include "BuffBreakdownModel.h"

class DebuffBreakdownModel : public BuffBreakdownModel
{
    Q_OBJECT
public:
    DebuffBreakdownModel(NumberCruncher* statistics_source);

private:
    void add_statistics() override;
};

#endif // DEBUFFBREAKDOWNMODEL_H
