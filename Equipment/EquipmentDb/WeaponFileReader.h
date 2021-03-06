#ifndef WEAPONFILEREADER_H
#define WEAPONFILEREADER_H

#include "ItemFileReader.h"

class WeaponFileReader: public ItemFileReader {
public:
    void weapon_file_handler(QXmlStreamReader &reader, QVector<Item*>& items);

private:
    void dmg_range_element_reader(const QXmlStreamAttributes &attrs, QMap<QString, QString>& item);
    void create_weapon(QVector<Item*>& items,
                       QMap<QString, QString>& item_map,
                       QVector<QPair<QString, QString>>& stats,
                       QVector<QMap<QString, QString>>& procs,
                       QVector<QMap<QString, QString>>& uses,
                       QVector<QString>& special_equip_effects);
    int get_weapon_type(const QString &type);
};

#endif // WEAPONFILEREADER_H
