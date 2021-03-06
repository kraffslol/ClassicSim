#include "TestHeroicStrike.h"

#include "Arms.h"
#include "Equipment.h"
#include "HeroicStrike.h"
#include "NoEffectBuff.h"
#include "Talent.h"
#include "WarriorSpells.h"

TestHeroicStrike::TestHeroicStrike(EquipmentDb *equipment_db) :
    TestSpellWarrior(equipment_db, "Heroic Strike")
{}

void TestHeroicStrike::test_all() {
    run_mandatory_tests();

    set_up();
    test_1_of_3_improved_hs_reduces_rage_cost();
    tear_down();

    set_up();
    test_2_of_3_improved_hs_reduces_rage_cost();
    tear_down();

    set_up();
    test_3_of_3_improved_hs_reduces_rage_cost();
    tear_down();

    set_up();
    test_removing_points_in_improved_hs_increases_rage_cost();
    tear_down();

    set_up();
    test_hit_dmg();
    tear_down();

    set_up();
    test_crit_dmg_0_of_2_impale();
    tear_down();

    set_up();
    test_crit_dmg_1_of_2_impale();
    tear_down();

    set_up();
    test_crit_dmg_2_of_2_impale();
    tear_down();

    set_up();
    test_dodge_applies_overpower_buff();
    tear_down();
}

HeroicStrike* TestHeroicStrike::heroic_strike() {
    return dynamic_cast<WarriorSpells*>(warrior->get_spells())->get_heroic_strike();
}

void TestHeroicStrike::test_name_correct() {
    assert(heroic_strike()->get_name() == "Heroic Strike");
}

void TestHeroicStrike::test_spell_cooldown() {
    assert(QString::number(heroic_strike()->get_base_cooldown(), 'f', 3) == "0.000");
}

void TestHeroicStrike::test_whether_spell_causes_global_cooldown() {
    assert(warrior->action_ready());

    when_heroic_strike_is_performed();

    assert(warrior->action_ready());
}

void TestHeroicStrike::test_how_spell_observes_global_cooldown() {
    given_warrior_has_rage(100);
    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);

    given_warrior_is_on_gcd();

    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);
}

void TestHeroicStrike::test_resource_cost() {
    then_heroic_strike_costs(15);
}

void TestHeroicStrike::test_is_ready_conditions() {
    given_warrior_in_battle_stance();
    given_warrior_has_rage(100);
    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);

    given_warrior_in_berserker_stance();
    given_warrior_has_rage(100);
    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);

    given_warrior_in_defensive_stance();
    given_warrior_has_rage(100);
    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);
}

void TestHeroicStrike::test_stance_cooldown() {
    when_switching_to_berserker_stance();

    given_warrior_has_rage(100);
    assert(warrior->on_stance_cooldown() == true);

    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);
}

void TestHeroicStrike::test_1_of_3_improved_hs_reduces_rage_cost() {
    given_1_of_3_improved_hs();

    then_heroic_strike_costs(14);
}

void TestHeroicStrike::test_2_of_3_improved_hs_reduces_rage_cost() {
    given_2_of_3_improved_hs();

    then_heroic_strike_costs(13);
}

void TestHeroicStrike::test_3_of_3_improved_hs_reduces_rage_cost() {
    given_3_of_3_improved_hs();

    then_heroic_strike_costs(12);
}

void TestHeroicStrike::test_removing_points_in_improved_hs_increases_rage_cost() {
    Talent* improved_heroic_strike = Arms(warrior).get_improved_heroic_strike();

    assert(improved_heroic_strike->increment_rank());
    assert(improved_heroic_strike->increment_rank());
    assert(improved_heroic_strike->increment_rank());

    assert(improved_heroic_strike->decrement_rank());
    then_heroic_strike_costs(13);

    assert(improved_heroic_strike->decrement_rank());
    then_heroic_strike_costs(14);

    assert(improved_heroic_strike->decrement_rank());
    then_heroic_strike_costs(15);

    delete improved_heroic_strike;
}

void TestHeroicStrike::test_hit_dmg() {
    given_target_has_0_armor();
    given_a_mainhand_weapon_with_100_min_max_dmg();
    given_1000_melee_ap();
    given_a_guaranteed_melee_ability_hit();
    given_no_previous_damage_dealt();
    given_2_of_2_impale();

    when_heroic_strike_is_performed();

    // [Damage] = base_dmg + (wpn_speed * AP / 14) + hs_flat_dmg
    // [443] = 100 + (2.6 * 1000 / 14) + 157
    then_damage_dealt_is(443);
}

void TestHeroicStrike::test_crit_dmg_0_of_2_impale() {
    given_target_has_0_armor();
    given_a_mainhand_weapon_with_100_min_max_dmg();
    given_1000_melee_ap();
    given_a_guaranteed_melee_ability_crit();
    given_no_previous_damage_dealt();
    given_0_of_2_impale();

    when_heroic_strike_is_performed();

    // [Damage] = (base_dmg + (wpn_speed * AP / 14) + hs_flat_dmg) * crit_dmg_modifier
    // [885] = (100 + (2.6 * 1000 / 14) + 157) * 2
    then_damage_dealt_is(885);
}
void TestHeroicStrike::test_crit_dmg_1_of_2_impale() {
    given_target_has_0_armor();
    given_a_mainhand_weapon_with_100_min_max_dmg();
    given_1000_melee_ap();
    given_a_guaranteed_melee_ability_crit();
    given_no_previous_damage_dealt();
    given_1_of_2_impale();

    when_heroic_strike_is_performed();

    // [Damage] = (base_dmg + (wpn_speed * AP / 14) + hs_flat_dmg) * crit_dmg_modifier
    // [930] = (100 + (2.6 * 1000 / 14) + 157) * 2.1
    then_damage_dealt_is(930);
}

void TestHeroicStrike::test_crit_dmg_2_of_2_impale() {
    given_target_has_0_armor();
    given_a_mainhand_weapon_with_100_min_max_dmg();
    given_1000_melee_ap();
    given_a_guaranteed_melee_ability_crit();
    given_no_previous_damage_dealt();
    given_2_of_2_impale();

    when_heroic_strike_is_performed();

    // [Damage] = (base_dmg + (wpn_speed * AP / 14) + hs_flat_dmg) * crit_dmg_modifier
    // [974] = (100 + (2.6 * 1000 / 14) + 157) * 2.2
    then_damage_dealt_is(974);
}

void TestHeroicStrike::test_dodge_applies_overpower_buff() {
    given_a_guaranteed_melee_ability_dodge();

    when_heroic_strike_is_performed();

    then_overpower_is_active();
}

void TestHeroicStrike::given_0_of_3_improved_hs() {
    then_heroic_strike_costs(15);
}

void TestHeroicStrike::given_1_of_3_improved_hs() {
    Talent* talent = Arms(warrior).get_improved_heroic_strike();

    assert(talent->increment_rank());

    delete talent;
}

void TestHeroicStrike::given_2_of_3_improved_hs() {
    Talent* talent = Arms(warrior).get_improved_heroic_strike();

    assert(talent->increment_rank());
    assert(talent->increment_rank());

    delete talent;
}

void TestHeroicStrike::given_3_of_3_improved_hs() {
    Talent* talent = Arms(warrior).get_improved_heroic_strike();

    assert(talent->increment_rank());
    assert(talent->increment_rank());
    assert(talent->increment_rank());

    delete talent;
}

void TestHeroicStrike::given_heroic_strike_is_queued() {
    heroic_strike()->perform();
    assert(heroic_strike()->is_queued());
}

void TestHeroicStrike::given_heroic_strike_is_not_queued() {
    heroic_strike()->cancel();
    assert(!heroic_strike()->is_queued());
}

void TestHeroicStrike::when_heroic_strike_is_performed() {
    if (pchar->get_equipment()->get_mainhand() == nullptr)
        given_a_mainhand_weapon_with_100_min_max_dmg();

    heroic_strike()->calculate_damage();
}

void TestHeroicStrike::then_heroic_strike_costs(const unsigned rage) {
    warrior->lose_rage(warrior->get_resource_level(ResourceType::Rage));
    warrior->gain_rage(rage);
    assert(heroic_strike()->get_spell_status() == SpellStatus::Available);

    warrior->lose_rage(warrior->get_resource_level(ResourceType::Rage));
    warrior->gain_rage(rage - 1);
    assert(heroic_strike()->get_spell_status() == SpellStatus::InsufficientResources);
}
