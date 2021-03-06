#include "CharacterSpells.h"

#include "Berserking.h"
#include "BloodFury.h"
#include "Character.h"
#include "CharacterStats.h"
#include "ClassStatistics.h"
#include "CooldownControl.h"
#include "DemonicRune.h"
#include "Engine.h"
#include "MainhandAttack.h"
#include "MainhandMeleeHit.h"
#include "ManaPotion.h"
#include "NightDragonsBreath.h"
#include "OffhandAttack.h"
#include "OffhandMeleeHit.h"
#include "Pet.h"
#include "Race.h"
#include "Rotation.h"
#include "SpellRankGroup.h"
#include "Target.h"
#include "Utils/Check.h"

CharacterSpells::CharacterSpells(Character* pchar) :
    pchar(pchar),
    rotation(nullptr),
    cast_is_in_progress(false),
    id_of_cast_in_progress(0),
    attack_mode(AttackMode::MeleeAttack),
    attack_mode_active(false),
    next_instance_id(SpellID::INITIAL_ID)
{
    berserking = new Berserking(pchar);
    blood_fury = new BloodFury(pchar);
    demonic_rune = new DemonicRune(pchar);
    mana_potion = new ManaPotion(pchar);
    night_dragons_breath = new NightDragonsBreath(pchar);

    add_spell_group({berserking});
    add_spell_group({blood_fury});
    add_spell_group({demonic_rune});
    add_spell_group({mana_potion});
    add_spell_group({night_dragons_breath});
}

CharacterSpells::~CharacterSpells() {
    for (const auto & spell : spells)
        delete spell;

    for (const auto & cc : cooldown_controls)
        delete cc;

    for (const auto & spell_group : spell_rank_groups)
        delete spell_group;
}

void CharacterSpells::set_rotation(Rotation* rotation) {
    this->rotation = rotation;
    this->rotation->link_spells(pchar);
    set_attack_mode(this->rotation->get_attack_mode());
}

void CharacterSpells::relink_spells() {
    if (rotation != nullptr)
        rotation->link_spells(pchar);
}

void CharacterSpells::perform_rotation() {
    if (rotation == nullptr || cast_is_in_progress)
        return;

    this->rotation->perform_rotation();
}

Rotation* CharacterSpells::get_rotation() {
    return this->rotation;
}

bool CharacterSpells::cast_in_progress() const {
    return this->cast_is_in_progress;
}

unsigned CharacterSpells::start_cast() {
    check(!cast_is_in_progress, "Cast in progress when starting new cast");

    cast_is_in_progress = true;

    return ++id_of_cast_in_progress;
}

void CharacterSpells::complete_cast(const unsigned cast_id) {
    check(cast_is_in_progress, "Cast not in progress when completing cast");
    check((id_of_cast_in_progress == cast_id), "Mismatched cast id");

    cast_is_in_progress = false;

    pchar->add_player_reaction_event();
}

void CharacterSpells::activate_racials() {
    switch (pchar->get_race()->get_race_int()) {
    case Races::Orc:
        blood_fury->enable();
        if (pchar->get_pet())
            pchar->get_pet()->increase_damage_modifier(5);
        break;
    case Races::Troll:
        berserking->enable();
        pchar->get_stats()->increase_dmg_vs_type(Target::CreatureType::Beast, 0.05);
        break;
    }
}

void CharacterSpells::deactivate_racials() {
    switch (pchar->get_race()->get_race_int()) {
    case Races::Orc:
        blood_fury->disable();
        if (pchar->get_pet())
            pchar->get_pet()->decrease_damage_modifier(5);
        break;
    case Races::Troll:
        berserking->disable();
        pchar->get_stats()->decrease_dmg_vs_type(Target::CreatureType::Beast, 0.05);
        break;
    }
}

void CharacterSpells::add_spell(Spell* spell, bool relink) {
    if (spell->get_instance_id() == SpellID::INACTIVE) {
        spell->set_instance_id(next_instance_id);
        ++next_instance_id;
    }

    spells.append(spell);

    if (relink)
        relink_spells();
}

void CharacterSpells::remove_spell(Spell* spell) {
    for (const auto & i : spells) {
        if (i->get_instance_id() == spell->get_instance_id()) {
            spells.removeOne(i);
            break;
        }
    }

    spell_rank_groups.remove(spell->get_name());

    relink_spells();
}

void CharacterSpells::add_start_of_combat_spell(Spell* spell) {
    if (spell->get_instance_id() == SpellID::INACTIVE) {
        spell->set_instance_id(next_instance_id);
        ++next_instance_id;
    }

    start_of_combat_spells.append(spell);
}

void CharacterSpells::remove_start_of_combat_spell(Spell* spell) {
    for (const auto & i : start_of_combat_spells) {
        if (i->get_instance_id() == spell->get_instance_id()) {
            start_of_combat_spells.removeOne(i);
            return;
        }
    }
}

void CharacterSpells::run_start_of_combat_spells() {
    for (const auto & spell : start_of_combat_spells)
        spell->perform_start_of_combat();
}

void CharacterSpells::add_spell_group(const QVector<Spell*> spell_group, const bool relink) {
    check(!spell_group.empty(), "Cannot add empty spell group");
    check(!spell_rank_groups.contains(spell_group[0]->get_name()),
            QString("%1 has already been added as a spell group").arg(spell_group[0]->get_name()).toStdString());

    spell_rank_groups[spell_group[0]->get_name()] = new SpellRankGroup(spell_group[0]->get_name(), spell_group);

    for (auto & spell : spell_group)
        add_spell(spell, relink);
}

SpellRankGroup* CharacterSpells::get_spell_rank_group_by_name(const QString& spell_name) const {
    if (spell_rank_groups.contains(spell_name))
        return spell_rank_groups[spell_name];

    return nullptr;
}

void CharacterSpells::reset() {
    cast_is_in_progress = false;
    id_of_cast_in_progress = 0;
    attack_mode_active = false;

    for (const auto & spell : spells)
        spell->reset();
}

void CharacterSpells::start_attack() {
    attack_mode_active = true;

    switch (attack_mode) {
    case AttackMode::MeleeAttack:
        return start_melee_attack();
    case AttackMode::RangedAttack:
        return add_next_ranged_attack();
    default:
        return;
    }
}

void CharacterSpells::start_melee_attack() {
    add_next_mh_attack();

    if (pchar->is_dual_wielding()) {
        add_next_oh_attack();
    }
}

void CharacterSpells::start_pet_attack() {

}

void CharacterSpells::stop_attack() {
    attack_mode_active = false;
}

bool CharacterSpells::is_melee_attacking() const {
    return attack_mode == AttackMode::MeleeAttack && attack_mode_active;
}

bool CharacterSpells::is_ranged_attacking() const {
    return attack_mode == AttackMode::RangedAttack && attack_mode_active;
}

void CharacterSpells::set_attack_mode(const AttackMode attack_mode) {
    check(!attack_mode_active, "Attack mode active when setting new attack mode");
    this->attack_mode = attack_mode;
}

AttackMode CharacterSpells::get_attack_mode() const {
    return this->attack_mode;
}

void CharacterSpells::mh_auto_attack(const int iteration) {
    if (!mh_attack->attack_is_valid(iteration))
        return;

    if (!is_melee_attacking())
        return;

    mh_attack->perform();

    add_next_mh_attack();
}

void CharacterSpells::add_next_mh_attack() {
    if (attack_mode != AttackMode::MeleeAttack)
        return;

    auto* new_event = new MainhandMeleeHit(this, get_mh_attack()->get_next_expected_use(), get_mh_attack()->get_next_iteration());
    pchar->get_engine()->add_event(new_event);
}

void CharacterSpells::oh_auto_attack(const int) {
    check(false, "Not implemented");
}

void CharacterSpells::add_next_oh_attack() {
    check(false, "Not implemented");
}

void CharacterSpells::ranged_auto_attack(const int) {
    check(false, "Not implemented");
}

void CharacterSpells::add_next_ranged_attack() {
    check(false, "Not implemented");
}

MainhandAttack* CharacterSpells::get_mh_attack() const {
    return this->mh_attack;
}

OffhandAttack* CharacterSpells::get_oh_attack() const {
    check(false, "Not implemented");
    return nullptr;
}

AutoShot* CharacterSpells::get_auto_shot() const {
    return nullptr;
}

Berserking* CharacterSpells::get_berserking() const {
    return this->berserking;
}

BloodFury* CharacterSpells::get_blood_fury() const {
    return this->blood_fury;
}

DemonicRune* CharacterSpells::get_demonic_rune() const {
    return this->demonic_rune;
}

ManaPotion* CharacterSpells::get_mana_potion() const {
    return this->mana_potion;
}

NightDragonsBreath* CharacterSpells::get_night_dragons_breath() const {
    return this->night_dragons_breath;
}

CooldownControl* CharacterSpells::new_cooldown_control(const QString& spell_name, const double cooldown) {
    if (cooldown_controls.contains(spell_name))
        return cooldown_controls[spell_name];

    const auto new_cc = new CooldownControl(pchar, cooldown);
    cooldown_controls[spell_name] = new_cc;
    return new_cc;
}

void CharacterSpells::prepare_set_of_combat_iterations() {
    cast_is_in_progress = false;
    id_of_cast_in_progress = 0;
    attack_mode_active = false;

    for (const auto & spell : spells)
        spell->prepare_set_of_combat_iterations();

    if (rotation)
        rotation->prepare_set_of_combat_iterations();
}
