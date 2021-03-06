#include "NumberCruncher.h"
#include "SimScaleModel.h"
#include "SimSettings.h"

SimScaleModel::SimScaleModel(SimSettings *sim_settings, QObject *parent)
    : QAbstractListModel(parent),
      sim_settings(sim_settings)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    sim_options = {
        SimOption::Name::ScaleAgility,
        SimOption::Name::ScaleStrength,
        SimOption::Name::ScaleHitChance,
        SimOption::Name::ScaleCritChance,
        SimOption::Name::ScaleMeleeAP,
        SimOption::Name::ScaleAxeSkill,
        SimOption::Name::ScaleDaggerSkill,
        SimOption::Name::ScaleMaceSkill,
        SimOption::Name::ScaleSwordSkill,
    };
    endInsertRows();
}

void SimScaleModel::toggleOption(const int option) {
    auto option_cast = static_cast<SimOption::Name>(option);

    layoutAboutToBeChanged();

    if (sim_settings->option_active(option_cast))
        sim_settings->remove_sim_option(option_cast);
    else
        sim_settings->add_sim_option(option_cast);

    layoutChanged();
}

int SimScaleModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return sim_options.count();
}

QVariant SimScaleModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= sim_options.count())
        return QVariant();

    if (role == NameRole)
        return get_name_for_option(sim_options[index.row()]);
    if (role == ActiveRole)
        return sim_settings->option_active(sim_options[index.row()]);
    if (role == EnumRole)
        return sim_options[index.row()];

    return QVariant();
}

QHash<int, QByteArray> SimScaleModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "_name";
    roles[ActiveRole] = "_active";
    roles[EnumRole] = "_enum";
    return roles;
}
