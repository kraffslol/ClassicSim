#include "SimControl.h"

#include <QDebug>

#include "Character.h"
#include "CharacterSpells.h"
#include "CharacterStats.h"
#include "ClassStatistics.h"
#include "CombatRoll.h"
#include "EncounterEnd.h"
#include "EncounterStart.h"
#include "Engine.h"
#include "ItemNamespace.h"
#include "NumberCruncher.h"
#include "Rotation.h"
#include "SpellCastingTime.h"

SimControl::SimControl(SimSettings* sim_settings, NumberCruncher *scaler) :
    sim_settings(sim_settings),
    scaler(scaler)
{}

void SimControl::run_quick_sim(Character* pchar) {
    run_sim(pchar, sim_settings->get_combat_length(), sim_settings->get_combat_iterations_quick_sim());

    scaler->add_class_statistic(SimOption::Name::NoScale, pchar->relinquish_ownership_of_statistics());
}

void SimControl::run_full_sim(Character* pchar) {
    run_sim(pchar, sim_settings->get_combat_length(), sim_settings->get_combat_iterations_full_sim());

    scaler->add_class_statistic(SimOption::Name::NoScale, pchar->relinquish_ownership_of_statistics());

    QSet<SimOption::Name> options = sim_settings->get_active_options();
    for (const auto & option : options) {
        qDebug() << "Running sim with option" << option;
        run_sim_with_option(pchar, option, sim_settings->get_combat_length(), sim_settings->get_combat_iterations_full_sim());
        scaler->add_class_statistic(option, pchar->relinquish_ownership_of_statistics());
    }
}

void SimControl::run_sim(Character* pchar, const int combat_length, const int iterations) {
    pchar->get_combat_roll()->drop_tables();
    pchar->prepare_set_of_combat_iterations();

    Rotation* rotation = pchar->get_spells()->get_rotation();

    for (int i = 0; i < iterations; ++i) {
        pchar->get_engine()->prepare_iteration(-rotation->get_time_required_to_run_precombat());
        rotation->run_precombat_actions();
        if (rotation->precast_spell != nullptr && rotation->precast_spell->is_enabled())
            rotation->precast_spell->perform();

        pchar->get_engine()->add_event(new EncounterStart(pchar->get_spells(), pchar->get_enabled_buffs()));
        pchar->get_engine()->add_event(new EncounterEnd(pchar->get_engine(), pchar, combat_length));
        pchar->get_engine()->run();
        pchar->get_statistics()->finish_combat_iteration();
    }

    rotation->finish_set_of_combat_iterations();
    pchar->get_engine()->reset();
}

void SimControl::run_sim_with_option(Character* pchar, SimOption::Name option, const int combat_length, const int iterations) {
    add_option(pchar, option);

    run_sim(pchar, combat_length, iterations);

    remove_option(pchar, option);
}

void SimControl::add_option(Character* pchar, SimOption::Name option) {
    switch (option) {
    case SimOption::Name::NoScale:
        break;
    case SimOption::Name::ScaleAgility:
        pchar->get_stats()->increase_agility(10);
        break;
    case SimOption::Name::ScaleStrength:
        pchar->get_stats()->increase_strength(10);
        break;
    case SimOption::Name::ScaleMeleeAP:
        pchar->get_stats()->increase_melee_ap(10);
        break;
    case SimOption::Name::ScaleHitChance:
        pchar->get_stats()->increase_melee_hit(100);
        break;
    case SimOption::Name::ScaleCritChance:
        pchar->get_stats()->increase_melee_crit(100);
        break;
    case SimOption::Name::ScaleAxeSkill:
        pchar->get_stats()->increase_wpn_skill(WeaponTypes::AXE, 1);
        break;
    case SimOption::Name::ScaleDaggerSkill:
        pchar->get_stats()->increase_wpn_skill(WeaponTypes::DAGGER, 1);
        break;
    case SimOption::Name::ScaleMaceSkill:
        pchar->get_stats()->increase_wpn_skill(WeaponTypes::MACE, 1);
        break;
    case SimOption::Name::ScaleSwordSkill:
        pchar->get_stats()->increase_wpn_skill(WeaponTypes::SWORD, 1);
        break;
    }

    pchar->get_statistics()->set_sim_option(option);
}

void SimControl::remove_option(Character* pchar, SimOption::Name option) {
    switch (option) {
    case SimOption::Name::NoScale:
        break;
    case SimOption::Name::ScaleAgility:
        pchar->get_stats()->decrease_agility(10);
        break;
    case SimOption::Name::ScaleStrength:
        pchar->get_stats()->decrease_strength(10);
        break;
    case SimOption::Name::ScaleMeleeAP:
        pchar->get_stats()->decrease_melee_ap(10);
        break;
    case SimOption::Name::ScaleHitChance:
        pchar->get_stats()->decrease_melee_hit(1);
        break;
    case SimOption::Name::ScaleCritChance:
        pchar->get_stats()->decrease_melee_crit(100);
        break;
    case SimOption::Name::ScaleAxeSkill:
        pchar->get_stats()->decrease_wpn_skill(WeaponTypes::AXE, 1);
        break;
    case SimOption::Name::ScaleDaggerSkill:
        pchar->get_stats()->decrease_wpn_skill(WeaponTypes::DAGGER, 1);
        break;
    case SimOption::Name::ScaleMaceSkill:
        pchar->get_stats()->decrease_wpn_skill(WeaponTypes::MACE, 1);
        break;
    case SimOption::Name::ScaleSwordSkill:
        pchar->get_stats()->decrease_wpn_skill(WeaponTypes::SWORD, 1);
        break;
    }
}
