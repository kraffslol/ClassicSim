#include "TestHunterTalentStatIncrease.h"

#include "BeastMastery.h"
#include "CharacterStats.h"
#include "Marksmanship.h"
#include "Pet.h"
#include "Survival.h"
#include "Talent.h"

TestHunterTalentStatIncrease::TestHunterTalentStatIncrease(EquipmentDb* equipment_db) :
    TestSpellHunter(equipment_db, "HunterTalentStatIncrease")
{}

void TestHunterTalentStatIncrease::test_all() {
    run_mandatory_tests();

    set_up();
    test_surefooted();
    tear_down();

    set_up();
    test_killer_instinct();
    tear_down();

    set_up();
    test_lightning_reflexes();
    tear_down();

    set_up();
    test_trueshot_aura();
    tear_down();

    set_up();
    test_unleashed_fury();
    tear_down();

    set_up();
    test_ferocity();
    tear_down();
}

void TestHunterTalentStatIncrease::test_lightning_reflexes() {
    Talent* talent = Survival(hunter).get_lightning_reflexes();
    given_character_has_agility(100);

    assert(talent->increment_rank());
    assert(pchar->get_stats()->get_agility() == 103);

    assert(talent->increment_rank());
    assert(pchar->get_stats()->get_agility() == 106);

    assert(talent->increment_rank());
    assert(pchar->get_stats()->get_agility() == 109);

    assert(talent->increment_rank());
    assert(pchar->get_stats()->get_agility() == 112);

    assert(talent->increment_rank());
    assert(pchar->get_stats()->get_agility() == 115);

    assert(talent->decrement_rank());
    assert(pchar->get_stats()->get_agility() == 112);

    assert(talent->decrement_rank());
    assert(pchar->get_stats()->get_agility() == 109);

    assert(talent->decrement_rank());
    assert(pchar->get_stats()->get_agility() == 106);

    assert(talent->decrement_rank());
    assert(pchar->get_stats()->get_agility() == 103);

    assert(talent->decrement_rank());
    assert(pchar->get_stats()->get_agility() == 100);

    delete talent;
}

void TestHunterTalentStatIncrease::test_surefooted() {
    Talent* talent = Survival(hunter).get_surefooted();
    unsigned melee_hit_initial = pchar->get_stats()->get_melee_hit_chance();
    unsigned ranged_hit_initial = pchar->get_stats()->get_ranged_hit_chance();

    assert(talent->increment_rank());
    assert(melee_hit_initial + 100 == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial + 100 == pchar->get_stats()->get_ranged_hit_chance());

    assert(talent->increment_rank());
    assert(melee_hit_initial + 200 == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial + 200 == pchar->get_stats()->get_ranged_hit_chance());

    assert(talent->increment_rank());
    assert(melee_hit_initial + 300 == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial + 300 == pchar->get_stats()->get_ranged_hit_chance());

    assert(talent->decrement_rank());
    assert(melee_hit_initial + 200 == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial + 200 == pchar->get_stats()->get_ranged_hit_chance());

    assert(talent->decrement_rank());
    assert(melee_hit_initial + 100 == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial + 100 == pchar->get_stats()->get_ranged_hit_chance());

    assert(talent->decrement_rank());
    assert(melee_hit_initial == pchar->get_stats()->get_melee_hit_chance());
    assert(ranged_hit_initial == pchar->get_stats()->get_ranged_hit_chance());

    delete talent;
}

void TestHunterTalentStatIncrease::test_killer_instinct() {
    Talent* talent = Survival(hunter).get_killer_instinct();
    unsigned melee_crit_initial = pchar->get_stats()->get_mh_crit_chance();
    unsigned ranged_crit_initial = pchar->get_stats()->get_ranged_crit_chance();

    assert(talent->increment_rank());
    assert(melee_crit_initial + 100 == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial + 100 == pchar->get_stats()->get_ranged_crit_chance());

    assert(talent->increment_rank());
    assert(melee_crit_initial + 200 == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial + 200 == pchar->get_stats()->get_ranged_crit_chance());

    assert(talent->increment_rank());
    assert(melee_crit_initial + 300 == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial + 300 == pchar->get_stats()->get_ranged_crit_chance());

    assert(talent->decrement_rank());
    assert(melee_crit_initial + 200 == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial + 200 == pchar->get_stats()->get_ranged_crit_chance());

    assert(talent->decrement_rank());
    assert(melee_crit_initial + 100 == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial + 100 == pchar->get_stats()->get_ranged_crit_chance());

    assert(talent->decrement_rank());
    assert(melee_crit_initial == pchar->get_stats()->get_mh_crit_chance());
    assert(ranged_crit_initial == pchar->get_stats()->get_ranged_crit_chance());

    delete talent;
}

void TestHunterTalentStatIncrease::test_trueshot_aura() {
    Talent* talent = Marksmanship(hunter).get_trueshot_aura();
    unsigned melee_ap_initial = pchar->get_stats()->get_melee_ap();
    unsigned ranged_ap_initial = pchar->get_stats()->get_ranged_ap();

    assert(talent->increment_rank());
    assert(melee_ap_initial + 50 == pchar->get_stats()->get_melee_ap());
    assert(ranged_ap_initial + 50 == pchar->get_stats()->get_ranged_ap());

    assert(talent->decrement_rank());
    assert(melee_ap_initial == pchar->get_stats()->get_melee_ap());
    assert(ranged_ap_initial == pchar->get_stats()->get_ranged_ap());

    delete talent;
}

void TestHunterTalentStatIncrease::test_unleashed_fury() {
    Talent* talent = BeastMastery(hunter).get_unleashed_fury();
    Pet* pet = hunter->get_pet();

    assert(almost_equal(1.05, pet->get_damage_modifier()));

    assert(talent->increment_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.092 = 1 * 1.05 * 1.04
    assert(almost_equal(1.092, pet->get_damage_modifier()));

    assert(talent->increment_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.134 = 1 * 1.05 * 1.08
    assert(almost_equal(1.134, pet->get_damage_modifier()));

    assert(talent->increment_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.176 = 1 * 1.05 * 1.12
    assert(almost_equal(1.176, pet->get_damage_modifier()));

    assert(talent->increment_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.218 = 1 * 1.05 * 1.16
    assert(almost_equal(1.218, pet->get_damage_modifier()));

    assert(talent->increment_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.26 = 1 * 1.05 * 1.20
    assert(almost_equal(1.26, pet->get_damage_modifier()));

    assert(talent->decrement_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.218 = 1 * 1.05 * 1.16
    assert(almost_equal(1.218, pet->get_damage_modifier()));

    assert(talent->decrement_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.176 = 1 * 1.05 * 1.12
    assert(almost_equal(1.176, pet->get_damage_modifier()));

    assert(talent->decrement_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.134 = 1 * 1.05 * 1.08
    assert(almost_equal(1.134, pet->get_damage_modifier()));

    assert(talent->decrement_rank());
    // DmgMod = base * orc_racial * unleashed_fury
    // 1.092 = 1 * 1.05 * 1.04
    assert(almost_equal(1.092, pet->get_damage_modifier()));

    assert(talent->decrement_rank());
    // DmgMod = base * orc_racial
    // 1.05 = 1 * 1.05
    assert(almost_equal(1.05, pet->get_damage_modifier()));

    delete talent;
}

void TestHunterTalentStatIncrease::test_ferocity() {
    Talent* talent = BeastMastery(hunter).get_ferocity();
    Pet* pet = hunter->get_pet();

    assert(pet->get_crit_chance() == 500);

    assert(talent->increment_rank());
    assert(pet->get_crit_chance() == 800);

    assert(talent->increment_rank());
    assert(pet->get_crit_chance() == 1100);

    assert(talent->increment_rank());
    assert(pet->get_crit_chance() == 1400);

    assert(talent->increment_rank());
    assert(pet->get_crit_chance() == 1700);

    assert(talent->increment_rank());
    assert(pet->get_crit_chance() == 2000);

    assert(talent->decrement_rank());
    assert(pet->get_crit_chance() == 1700);

    assert(talent->decrement_rank());
    assert(pet->get_crit_chance() == 1400);

    assert(talent->decrement_rank());
    assert(pet->get_crit_chance() == 1100);

    assert(talent->decrement_rank());
    assert(pet->get_crit_chance() == 800);

    assert(talent->decrement_rank());
    assert(pet->get_crit_chance() == 500);

    delete talent;
}

void TestHunterTalentStatIncrease::test_name_correct() {
}

void TestHunterTalentStatIncrease::test_spell_cooldown() {
}

void TestHunterTalentStatIncrease::test_how_spell_observes_global_cooldown() {
}

void TestHunterTalentStatIncrease::test_resource_cost() {
}

void TestHunterTalentStatIncrease::test_is_ready_conditions() {
}

void TestHunterTalentStatIncrease::test_whether_spell_causes_global_cooldown() {
}
