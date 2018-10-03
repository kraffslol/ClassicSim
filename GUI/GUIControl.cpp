
#include "GUIControl.h"

#include "Dwarf.h"
#include "Gnome.h"
#include "Human.h"
#include "NightElf.h"
#include "Orc.h"
#include "Tauren.h"
#include "Troll.h"
#include "Undead.h"

#include "Druid.h"
#include "Hunter.h"
#include "Mage.h"
#include "Paladin.h"
#include "Priest.h"
#include "Rogue.h"
#include "Shaman.h"
#include "Warlock.h"
#include "Warrior.h"

#include "Spells.h"

#include "EncounterStart.h"
#include "EncounterEnd.h"

#include "Weapon.h"
#include "WeaponModel.h"
#include "EquipmentDb.h"

#include "CharacterEncoder.h"
#include "CharacterDecoder.h"
#include "CharacterStats.h"
#include "ClassStatistics.h"

#include "SimulationThreadPool.h"

#include "ActiveBuffs.h"
#include "BuffModel.h"
#include "GeneralBuffs.h"
#include "RotationFileReader.h"
#include "Rotation.h"

#include <QDebug>
#include <utility>

GUIControl::GUIControl(QObject* parent) :
    QObject(parent),
    last_quick_sim_result(0.0)
{
    QObject::connect(this, SIGNAL(startQuickSim()), this, SLOT(run_quick_sim()));

    races.insert("Dwarf", new Dwarf());
    races.insert("Gnome", new Gnome());
    races.insert("Human", new Human());
    races.insert("Night Elf", new NightElf());
    races.insert("Orc", new Orc());
    races.insert("Tauren", new Tauren());
    races.insert("Troll", new Troll());
    races.insert("Undead", new Undead());

    engine = new Engine();
    equipment = new Equipment();
    target = new Target(63);
    combat = new CombatRoll(target);
    faction = new Faction();

    item_type_filter_model = new ItemTypeFilterModel();
    active_stat_filter_model = new ActiveItemStatFilterModel();
    item_model = new ItemModel(equipment->get_db(), item_type_filter_model, active_stat_filter_model);
    weapon_model = new WeaponModel(equipment->get_db(), item_type_filter_model, active_stat_filter_model);
    active_stat_filter_model->set_item_model(item_model);
    active_stat_filter_model->set_weapon_model(weapon_model);
    available_stat_filter_model = new AvailableItemStatFilterModel(active_stat_filter_model);

    chars.insert("Druid", dynamic_cast<Character*>(new Druid(races["Night Elf"], engine, equipment, combat, faction)));
    chars.insert("Hunter", dynamic_cast<Character*>(new Hunter(races["Dwarf"], engine, equipment, combat, faction)));
    chars.insert("Mage", dynamic_cast<Character*>(new Mage(races["Gnome"], engine, equipment, combat, faction)));
    chars.insert("Paladin", dynamic_cast<Character*>(new Paladin(races["Human"], engine, equipment, combat, faction)));
    chars.insert("Priest", dynamic_cast<Character*>(new Priest(races["Undead"], engine, equipment, combat, faction)));
    chars.insert("Rogue", dynamic_cast<Character*>(new Rogue(races["Troll"], engine, equipment, combat, faction)));
    chars.insert("Shaman", dynamic_cast<Character*>(new Shaman(races["Tauren"], engine, equipment, combat, faction)));
    chars.insert("Warlock", dynamic_cast<Character*>(new Warlock(races["Orc"], engine, equipment, combat, faction)));
    chars.insert("Warrior", dynamic_cast<Character*>(new Warrior(races["Orc"], engine, equipment, combat, faction)));

    set_character(chars["Warrior"]);

    // TODO: Handle switching pchar
    character_encoder = new CharacterEncoder(current_char);
    character_decoder = new CharacterDecoder();
    thread_pool = new SimulationThreadPool();
    // TODO: Handle switching pchar
    buff_model = new BuffModel(current_char->get_active_buffs()->get_general_buffs());
    debuff_model = new DebuffModel(current_char->get_active_buffs()->get_general_buffs());

    item_model->addItems(equipment->get_db());
    weapon_model->addWeapons(equipment->get_db());
}

GUIControl::~GUIControl() {
    QMap<QString, Character*>::const_iterator it_chars = chars.constBegin();
    auto end_chars = chars.constEnd();
    while(it_chars != end_chars) {
        delete it_chars.value();
        ++it_chars;
    }

    chars.clear();

    QMap<QString, Race*>::const_iterator it_races = races.constBegin();
    auto end_races = races.constEnd();
    while(it_races != end_races) {
        delete it_races.value();
        ++it_races;
    }

    races.clear();

    delete engine;
    delete equipment;
    delete target;
    delete combat;
    delete faction;
    delete item_model;
    delete item_type_filter_model;
    delete active_stat_filter_model;
    delete available_stat_filter_model;
    delete weapon_model;
    delete buff_model;
    delete debuff_model;
    delete character_encoder;
    delete character_decoder;
    delete thread_pool;
}

void GUIControl::set_character(Character* pchar) {
    current_char = pchar;
    item_type_filter_model->set_character(current_char);
    equipment->set_character(current_char);
}

void GUIControl::selectClass(const QString& class_name) {
    if (!chars.contains(class_name)) {
        qDebug() << QString("Class %1 not found in char list!").arg(class_name);
        return;
    }

    if (class_name != "Warrior") {
        qDebug() << QString("Class %1 not implemented").arg(class_name);
        return;
    }

    set_character(chars[class_name]);
    raceChanged();
    classChanged();
    statsChanged();
}

void GUIControl::selectRace(const QString& race_name) {
    if (!races.contains(race_name)) {
        qDebug() << QString("Race %1 not found").arg(race_name);
        return;
    }

    if (!current_char->race_available(races[race_name])) {
        qDebug() << QString("Race %1 not available for %2").arg(race_name, current_char->get_name());
        return;
    }

    current_char->set_race(races[race_name]);
    raceChanged();
    statsChanged();
}

void GUIControl::selectFaction(const bool faction) {
    if (this->faction->get_faction() == faction)
        return;

    this->faction->switch_faction();

    QMap<QString, Character*>::const_iterator it_chars = chars.constBegin();
    auto end_chars = chars.constEnd();
    while(it_chars != end_chars) {
        it_chars.value()->switch_faction();
        reset_race(it_chars.value());
        ++it_chars;
    }

    factionChanged();

    if (current_char->get_name() == "Shaman" || current_char->get_name() == "Paladin") {
        set_character(chars["Warrior"]);
        reset_race(current_char);
        classChanged();
    }
}

bool GUIControl::raceAvailable(const QString& race_name) {
    if (!races.contains(race_name)) {
        qDebug() << QString("Race %1 not found").arg(race_name);
        return false;
    }

    return current_char->race_available(races[race_name]);
}

void GUIControl::reset_race(Character* pchar) {
    const QVector<QString>& available_races = faction->get_faction_races();

    for (const auto & available_race : available_races) {
        if (!races.contains(available_race)) {
            qDebug() << QString("Race %1 not valid!").arg(available_race);
            continue;
        }

        if (pchar->race_available(races[available_race])) {
            pchar->set_race(races[available_race]);
            break;
        }
    }

    raceChanged();
    statsChanged();
}

QString GUIControl::get_creature_type() const {
    return target->get_creature_type_string();
}

void GUIControl::setCreatureType(const QString& creature_type) {
    // TODO: Must update target for all chars
    current_char->change_target_creature_type(creature_type);
    creatureTypeChanged();
    statsChanged();
}

QString GUIControl::getLeftBackgroundImage() const {
    return current_char->get_talents()->get_background_image("LEFT");
}

QString GUIControl::getMidBackgroundImage() const {
    return current_char->get_talents()->get_background_image("MID");
}

QString GUIControl::getRightBackgroundImage() const {
    return current_char->get_talents()->get_background_image("RIGHT");
}

QString GUIControl::getIcon(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_icon(tree_position, talent_position);
}

bool GUIControl::showPosition(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_position(tree_position, talent_position);
}

bool GUIControl::showBottomArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_bottom_arrow(tree_position, talent_position);
}
bool GUIControl::showRightArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_right_arrow(tree_position, talent_position);
}

QString GUIControl::getBottomArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_bottom_arrow(tree_position, talent_position);
}

QString GUIControl::getRightArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_right_arrow(tree_position, talent_position);
}

bool GUIControl::bottomChildAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->bottom_child_available(tree_position, talent_position);
}

bool GUIControl::bottomChildActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->bottom_child_active(tree_position, talent_position);
}

bool GUIControl::rightChildAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->right_child_available(tree_position, talent_position);
}

bool GUIControl::rightChildActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->right_child_active(tree_position, talent_position);
}

bool GUIControl::isActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_active(tree_position, talent_position);
}

bool GUIControl::isAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_available(tree_position, talent_position);
}

bool GUIControl::isMaxed(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_maxed(tree_position, talent_position);
}

bool GUIControl::hasTalentPointsRemaining() const {
    return current_char->get_talents()->has_talent_points_remaining();
}

QString GUIControl::getRank(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_rank(tree_position, talent_position);
}

QString GUIControl::getMaxRank(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_max_rank(tree_position, talent_position);
}

void GUIControl::incrementRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->increment_rank(tree_position, talent_position);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

void GUIControl::decrementRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->decrement_rank(tree_position, talent_position);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

QString GUIControl::getRequirements(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_requirements(tree_position, talent_position);
}

QString GUIControl::getCurrentRankDescription(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_current_rank_description(tree_position, talent_position);
}

QString GUIControl::getNextRankDescription(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_next_rank_description(tree_position, talent_position);
}

int GUIControl::getTreePoints(const QString& tree_position) const {
    return current_char->get_talents()->get_tree_points(tree_position);
}

QString GUIControl::getTreeName(const QString& tree_position) const {
    return current_char->get_talents()->get_tree_name(tree_position);
}

QString GUIControl::getTalentName(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_talent_name(tree_position, talent_position);
}

void GUIControl::maxRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->increase_to_max_rank(tree_position, talent_position);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

void GUIControl::minRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->decrease_to_min_rank(tree_position, talent_position);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

void GUIControl::clearTree(const QString& tree_position) {
    current_char->get_talents()->clear_tree(tree_position);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

void GUIControl::setTalentSetup(const int talent_index) {
    current_char->get_talents()->set_current_index(talent_index);
    Q_EMIT talentsUpdated();
    Q_EMIT statsChanged();
}

int GUIControl::get_talent_points_remaining() const {
    return current_char->get_talents()->get_talent_points_remaining();
}

QString GUIControl::get_class_color() const {
    return current_char->get_class_color();
}

QString GUIControl::get_class_name() const {
    return current_char->get_name();
}

QString GUIControl::get_race_name() const {
    return current_char->get_race()->get_name();
}

bool GUIControl::get_is_alliance() const {
    return faction->is_alliance();
}

bool GUIControl::get_is_horde() const {
    return faction->is_horde();
}

int GUIControl::get_strength() const {
    return current_char->get_stats()->get_strength();
}

int GUIControl::get_agility() const {
    return current_char->get_stats()->get_agility();
}

int GUIControl::get_stamina() const {
    return current_char->get_stats()->get_stamina();
}

int GUIControl::get_intellect() const {
    return current_char->get_stats()->get_intellect();
}

int GUIControl::get_spirit() const {
    return current_char->get_stats()->get_spirit();
}

QString GUIControl::get_crit_chance() const {
    return QString::number(current_char->get_stats()->get_crit_chance() * 100, 'f', 2);
}

QString GUIControl::get_hit_chance() const {
    return QString::number(current_char->get_stats()->get_hit_chance() * 100, 'f', 2);
}

int GUIControl::get_attack_power() const {
    return current_char->get_stats()->get_melee_ap();
}

int GUIControl::get_mainhand_wpn_skill() const {
    return current_char->get_mh_wpn_skill();
}

int GUIControl::get_offhand_wpn_skill() const {
    return current_char->get_oh_wpn_skill();
}

ItemModel* GUIControl::get_item_model() const {
    return this->item_model;
}

WeaponModel* GUIControl::get_weapon_model() const {
    return this->weapon_model;
}

ItemTypeFilterModel* GUIControl::get_item_type_filter_model() const {
    return this->item_type_filter_model;
}

ActiveItemStatFilterModel* GUIControl::get_active_stat_filter_model() const {
    return this->active_stat_filter_model;
}

AvailableItemStatFilterModel* GUIControl::get_available_stat_filter_model() const {
    return this->available_stat_filter_model;
}

bool GUIControl::getFilterActive(const int filter) const {
    return this->item_type_filter_model->get_filter_active(filter);
}

void GUIControl::toggleSingleFilter(const int filter) {
    this->item_type_filter_model->toggle_single_filter(filter);
    this->item_model->update_items();
    this->weapon_model->update_items();
    Q_EMIT filtersUpdated();
}

void GUIControl::clearFiltersAndSelectSingle(const int filter) {
    this->item_type_filter_model->clear_filters_and_select_single_filter(filter);
    this->item_model->update_items();
    this->weapon_model->update_items();
    Q_EMIT filtersUpdated();
}

void GUIControl::selectRangeOfFiltersFromPrevious(const int filter) {
    this->item_type_filter_model->select_range_of_filters(filter);
    this->item_model->update_items();
    this->weapon_model->update_items();
    Q_EMIT filtersUpdated();
}

BuffModel* GUIControl::get_buff_model() const {
    return this->buff_model;
}

DebuffModel* GUIControl::get_debuff_model() const {
    return this->debuff_model;
}

void GUIControl::selectBuff(const QString& buff) {
    current_char->get_active_buffs()->get_general_buffs()->toggle_external_buff(buff);
    Q_EMIT statsChanged();
    Q_EMIT externalBuffsChanged();
}

bool GUIControl::buffActive(const QString& buff) const {
    return current_char->get_active_buffs()->get_general_buffs()->buff_active(buff);
}

void GUIControl::selectDebuff(const QString& debuff) {
    current_char->get_active_buffs()->get_general_buffs()->toggle_external_debuff(debuff);
    Q_EMIT externalDebuffsChanged();
}

bool GUIControl::debuffActive(const QString& debuff) const {
    return current_char->get_active_buffs()->get_general_buffs()->debuff_active(debuff);
}

void GUIControl::setBuffSetup(const int buff_index) {
    current_char->get_active_buffs()->get_general_buffs()->change_setup(buff_index);
    Q_EMIT statsChanged();
    Q_EMIT externalBuffsChanged();
    Q_EMIT externalDebuffsChanged();
}

int GUIControl::getNumStatisticsRows() const {
    return current_char->get_statistics()->getNumStatisticsRows();
}

QVariantList GUIControl::getChartInfo(const int index) const {
    return current_char->get_statistics()->getChartInfo(index);
}

QVariantList GUIControl::getTableInfo(const int index) const {
    return current_char->get_statistics()->getTableInfo(index);
}

QString GUIControl::getEntryIcon(const int index) const {
    return current_char->get_statistics()->getEntryIcon(index);
}

void GUIControl::run_quick_sim() {
    thread_pool->run_sim(character_encoder->get_current_setup_string());

    current_char->dump();
    current_char->get_statistics()->reset_statistics();
    engine->prepare();
    combat->drop_tables();
    // TODO: Remove hardcoded 1000 iterations for quick sim.
    for (int i = 0; i < 1000; ++i) {
        auto* start_event = new EncounterStart(current_char);
        auto* end_event = new EncounterEnd(engine, current_char);

        engine->add_event(end_event);
        engine->add_event(start_event);
        engine->run();
    }

    engine->dump();
    engine->reset();
    // TODO: Remove hardcoded 1000 iterations 300 seconds fight for quick sim.
    double previous = last_quick_sim_result;
    last_quick_sim_result = double(current_char->get_statistics()->get_total_damage_dealt()) / (1000 * 300);

    double delta = ((last_quick_sim_result - previous) / previous);
    QString change = delta > 0 ? "+" : "";
    change += QString::number(((last_quick_sim_result - previous) / previous) * 100, 'f', 1) + "%";

    QString dps = QString::number(last_quick_sim_result, 'f', 2);
    qDebug() << "Total DPS: " << dps;
    quickSimChanged(dps, change, delta > 0);
    current_char->dump();
    statisticsReady();
}

void GUIControl::runQuickSim() {
    statisticsCleared();
    startQuickSim();
}

QString GUIControl::get_mainhand_icon() const {
    if (equipment->get_mainhand() != nullptr)
        return "Assets/items/" + equipment->get_mainhand()->get_value("icon");
    return "";
}

QString GUIControl::get_offhand_icon() const {
    if (equipment->get_offhand() != nullptr)
        return "Assets/items/" + equipment->get_offhand()->get_value("icon");
    return "";
}

QString GUIControl::get_ranged_icon() const {
    if (equipment->get_ranged() != nullptr)
        return "Assets/items/" + equipment->get_ranged()->get_value("icon");
    return "";
}

QString GUIControl::get_head_icon() const {
    if (equipment->get_head() != nullptr)
        return "Assets/items/" + equipment->get_head()->get_value("icon");
    return "";
}

QString GUIControl::get_neck_icon() const {
    if (equipment->get_neck() != nullptr)
        return "Assets/items/" + equipment->get_neck()->get_value("icon");
    return "";
}

QString GUIControl::get_shoulders_icon() const {
    if (equipment->get_shoulders() != nullptr)
        return "Assets/items/" + equipment->get_shoulders()->get_value("icon");
    return "";
}

QString GUIControl::get_back_icon() const {
    if (equipment->get_back() != nullptr)
        return "Assets/items/" + equipment->get_back()->get_value("icon");
    return "";
}

QString GUIControl::get_chest_icon() const {
    if (equipment->get_chest() != nullptr)
        return "Assets/items/" + equipment->get_chest()->get_value("icon");
    return "";
}

QString GUIControl::get_wrist_icon() const {
    if (equipment->get_wrist() != nullptr)
        return "Assets/items/" + equipment->get_wrist()->get_value("icon");
    return "";
}

QString GUIControl::get_gloves_icon() const {
    if (equipment->get_gloves() != nullptr)
        return "Assets/items/" + equipment->get_gloves()->get_value("icon");
    return "";
}

QString GUIControl::get_belt_icon() const {
    if (equipment->get_belt() != nullptr)
        return "Assets/items/" + equipment->get_belt()->get_value("icon");
    return "";
}

QString GUIControl::get_legs_icon() const {
    if (equipment->get_legs() != nullptr)
        return "Assets/items/" + equipment->get_legs()->get_value("icon");
    return "";
}

QString GUIControl::get_boots_icon() const {
    if (equipment->get_boots() != nullptr)
        return "Assets/items/" + equipment->get_boots()->get_value("icon");
    return "";
}

QString GUIControl::get_ring1_icon() const {
    if (equipment->get_ring1() != nullptr)
        return "Assets/items/" + equipment->get_ring1()->get_value("icon");
    return "";
}

QString GUIControl::get_ring2_icon() const {
    if (equipment->get_ring2() != nullptr)
        return "Assets/items/" + equipment->get_ring2()->get_value("icon");
    return "";
}

QString GUIControl::get_trinket1_icon() const {
    if (equipment->get_trinket1() != nullptr)
        return "Assets/items/" + equipment->get_trinket1()->get_value("icon");
    return "";
}

QString GUIControl::get_trinket2_icon() const {
    if (equipment->get_trinket2() != nullptr)
        return "Assets/items/" + equipment->get_trinket2()->get_value("icon");
    return "";
}

void GUIControl::selectSlot(const QString& slot_string) {
    int slot = get_slot_int(slot_string);

    if (slot == -1)
        return;

    item_type_filter_model->set_item_slot(slot);

    switch (slot) {
    case ItemSlots::MAINHAND:
    case ItemSlots::OFFHAND:
    case ItemSlots::RANGED:
        weapon_model->setSlot(slot);
        break;
    default:
        item_model->setSlot(slot);
        break;
    }

    Q_EMIT equipmentSlotSelected();
}

void GUIControl::setSlot(const QString& slot_string, const QString& item) {
    // TODO: Replace with switch on slot as int.
    if (slot_string == "MAINHAND")
        equipment->set_mainhand(item);
    if (slot_string == "OFFHAND")
        equipment->set_offhand(item);
    if (slot_string == "RANGED")
        equipment->set_ranged(item);
    if (slot_string == "HEAD")
        equipment->set_head(item);
    if (slot_string == "NECK")
        equipment->set_neck(item);
    if (slot_string == "SHOULDERS")
        equipment->set_shoulders(item);
    if (slot_string == "BACK")
        equipment->set_back(item);
    if (slot_string == "CHEST")
        equipment->set_chest(item);
    if (slot_string == "WRIST")
        equipment->set_wrist(item);
    if (slot_string == "GLOVES")
        equipment->set_gloves(item);
    if (slot_string == "BELT")
        equipment->set_belt(item);
    if (slot_string == "LEGS")
        equipment->set_legs(item);
    if (slot_string == "BOOTS")
        equipment->set_boots(item);
    if (slot_string == "RING1")
        equipment->set_ring1(item);
    if (slot_string == "RING2")
        equipment->set_ring2(item);
    if (slot_string == "TRINKET1")
        equipment->set_trinket1(item);
    if (slot_string == "TRINKET2")
        equipment->set_trinket2(item);

    equipmentChanged();
    statsChanged();
}

void GUIControl::clearSlot(const QString& slot_string) {
    if (slot_string == "MAINHAND")
        equipment->clear_mainhand();
    if (slot_string == "OFFHAND")
        equipment->clear_offhand();
    if (slot_string == "RANGED")
        equipment->clear_ranged();
    if (slot_string == "HEAD")
        equipment->clear_head();
    if (slot_string == "NECK")
        equipment->clear_neck();
    if (slot_string == "SHOULDERS")
        equipment->clear_shoulders();
    if (slot_string == "BACK")
        equipment->clear_back();
    if (slot_string == "CHEST")
        equipment->clear_chest();
    if (slot_string == "WRIST")
        equipment->clear_wrist();
    if (slot_string == "GLOVES")
        equipment->clear_gloves();
    if (slot_string == "BELT")
        equipment->clear_belt();
    if (slot_string == "LEGS")
        equipment->clear_legs();
    if (slot_string == "BOOTS")
        equipment->clear_boots();
    if (slot_string == "RING1")
        equipment->clear_ring1();
    if (slot_string == "RING2")
        equipment->clear_ring2();
    if (slot_string == "TRINKET1")
        equipment->clear_trinket1();
    if (slot_string == "TRINKET2")
        equipment->clear_trinket2();

    equipmentChanged();
    statsChanged();
}

void GUIControl::setEquipmentSetup(const int equipment_index) {
    current_char->get_stats()->get_equipment()->change_setup(equipment_index);
    equipmentChanged();
    statsChanged();
}

void GUIControl::setPatch(const QString& patch) {
    weapon_model->set_patch(patch);
    item_model->set_patch(patch);
    buff_model->set_patch(patch);

    current_char->get_stats()->get_equipment()->clear_items_not_available_on_patch();
    equipmentChanged();
    statsChanged();
}

QVariantList GUIControl::getTooltip(const QString &slot_string) {
    Item* item = nullptr;

    if (slot_string == "MAINHAND")
        item = equipment->get_mainhand();
    if (slot_string == "OFFHAND")
        item = equipment->get_offhand();
    if (slot_string == "RANGED")
        item = equipment->get_ranged();
    if (slot_string == "HEAD")
        item = equipment->get_head();
    if (slot_string == "NECK")
        item = equipment->get_neck();
    if (slot_string == "SHOULDERS")
        item = equipment->get_shoulders();
    if (slot_string == "BACK")
        item = equipment->get_back();
    if (slot_string == "CHEST")
        item = equipment->get_chest();
    if (slot_string == "WRIST")
        item = equipment->get_wrist();
    if (slot_string == "GLOVES")
        item = equipment->get_gloves();
    if (slot_string == "BELT")
        item = equipment->get_belt();
    if (slot_string == "LEGS")
        item = equipment->get_legs();
    if (slot_string == "BOOTS")
        item = equipment->get_boots();
    if (slot_string == "RING1")
        item = equipment->get_ring1();
    if (slot_string == "RING2")
        item = equipment->get_ring2();
    if (slot_string == "TRINKET1")
        item = equipment->get_trinket1();
    if (slot_string == "TRINKET2")
        item = equipment->get_trinket2();

    if (item == nullptr)
        return QVariantList();

    QString boe_string = item->get_value("boe") == "yes" ? "Binds when equipped" :
                                                          "Binds when picked up";
    QString unique = item->get_value("unique") == "yes" ? "Unique" :
                                                          "";

    QString slot = item->get_value("slot");
    QString dmg_range = "";
    QString weapon_speed = "";
    QString dps = "";

    if (slot == "1H")
        set_weapon_tooltip(item, slot, "One-hand", dmg_range, weapon_speed, dps);
    else if (slot == "MH")
        set_weapon_tooltip(item, slot, "Mainhand", dmg_range, weapon_speed, dps);
    else if (slot == "OH")
        set_weapon_tooltip(item, slot, "Offhand", dmg_range, weapon_speed, dps);
    else if (slot == "2H")
        set_weapon_tooltip(item, slot, "Two-hand", dmg_range, weapon_speed, dps);
    else if (slot == "RING")
        slot = "Finger";
    else
        slot = get_capitalized_string(slot);

    QString class_restriction = "";
    set_class_restriction_tooltip(item, class_restriction);

    QString lvl_req = QString("Requires level %1").arg(item->get_value("req_lvl"));

    QVariantList tooltip_info = {
        QVariant(item->get_name()),
        QVariant(item->get_value("quality")),
        QVariant(boe_string),
        QVariant(unique),
        QVariant(slot),
        QVariant(get_capitalized_string(item->get_value("type"))),
        QVariant(dmg_range),
        QVariant(weapon_speed),
        QVariant(dps),
        QVariant(item->get_base_stat_tooltip()),
        QVariant(class_restriction),
        QVariant(lvl_req),
        QVariant(item->get_equip_effect_tooltip()),
        QVariant(item->get_value("special_equip_effect")),
        QVariant(item->get_value("flavour_text"))
    };

    return tooltip_info;
}

void GUIControl::set_weapon_tooltip(Item*& item, QString& slot, QString type, QString& dmg_range, QString& wpn_speed, QString& dps) {
    slot = std::move(type);
    auto* wpn = dynamic_cast<Weapon*>(item);
    dmg_range = QString("%1 - %2 Damage").arg(QString::number(wpn->get_min_dmg()), QString::number(wpn->get_max_dmg()));
    dps = QString("(%1 damage per second)").arg(QString::number(wpn->get_wpn_dps(), 'f', 1));
    wpn_speed = "Speed " + QString::number(wpn->get_base_weapon_speed(), 'f', 2);
}

void GUIControl::set_class_restriction_tooltip(Item *&item, QString &restriction) {
    QVector<QString> restrictions;

    if (item->get_value("RESTRICTED_TO_WARRIOR") != "")
        restrictions.append(QString("<font color=\"%1\">%2</font>").arg(chars["Warrior"]->get_class_color(), chars["Warrior"]->get_name()));
    if (item->get_value("RESTRICTED_TO_PALADIN") != "")
        restrictions.append(QString("<font color=\"%1\">%2</font>").arg(chars["Paladin"]->get_class_color(), chars["Paladin"]->get_name()));
    if (item->get_value("RESTRICTED_TO_ROGUE") != "")
        restrictions.append(QString("<font color=\"%1\">%2</font>").arg(chars["Rogue"]->get_class_color(), chars["Rogue"]->get_name()));
    if (item->get_value("RESTRICTED_TO_HUNTER") != "")
        restrictions.append(QString("<font color=\"%1\">%2</font>").arg(chars["Hunter"]->get_class_color(), chars["Hunter"]->get_name()));

    if (restrictions.empty())
        return;

    for (const auto & i : restrictions) {
        if (restriction == "")
            restriction = "Classes: ";
        else
            restriction += ", ";
        restriction += i;
    }
}

QString GUIControl::get_capitalized_string(const QString& string) const {
    return QString(string[0]) + QString(string.right(string.size() - 1)).toLower();
}
