#include "Combat.h"

#include "AdrenalineRush.h"
#include "Backstab.h"
#include "BladeFlurry.h"
#include "DaggerSpecialization.h"
#include "Eviscerate.h"
#include "FistWeaponSpecialization.h"
#include "MaceSpecialization.h"
#include "OffhandAttackRogue.h"
#include "Precision.h"
#include "Rogue.h"
#include "RogueSpells.h"
#include "SinisterStrike.h"
#include "SwordSpecialization.h"
#include "Talent.h"
#include "WeaponExpertise.h"

Combat::Combat(Character *pchar) :
    TalentTree("Combat", "Assets/rogue/rogue_combat.jpg"),
    rogue(dynamic_cast<Rogue*>(pchar)),
    spells(dynamic_cast<RogueSpells*>(pchar->get_spells()))
{
    QMap<QString, Talent*> tier1 {{"1LL", new Talent(pchar, this, "Improved Gouge", "1LL", "Assets/ability/Ability_gouge.png", 3, "Increases the effect duration of your Gouge ability by %1 sec.", QVector<QPair<double, double>>{{0.5, 0.5}})},
                                  {"1ML", get_improved_sinister_strike()},
                                  {"1MR", new Talent(pchar, this, "Lightning Reflexes", "1MR", "Assets/spell/Spell_nature_invisibility.png", 5, "Increases your Dodge chance by %1%.", QVector<QPair<unsigned, unsigned>>{{1, 1}})}};
    add_talents(tier1);

    QMap<QString, Talent*> tier2 {{"2LL", get_improved_backstab()},
                                  {"2ML", new Talent(pchar, this, "Deflection", "2ML", "Assets/ability/Ability_parry.png", 5, "Increases your Parry chance by %1%.", QVector<QPair<unsigned, unsigned>>{{1, 1}})},
                                  {"2MR", new Precision(pchar, this)}};
    add_talents(tier2);

    QMap<QString, Talent*> tier3 {{"3LL", get_endurance()},
                                  {"3ML", new Talent(pchar, this, "Riposte", "3ML", "Assets/ability/Ability_warrior_challange.png", 1, "A strike that becomes active after parrying an opponent's attack. This deals 150% weapon damage and disarms the target for 6 sec.", QVector<QPair<unsigned, unsigned>>{})},
                                  {"3RR", new Talent(pchar, this, "Improved Sprint", "3RR", "Assets/ability/Ability_rogue_sprint.png", 2, "Gives a %1% chance to remove all movement impairing effects when you activate your Sprint ability.", QVector<QPair<unsigned, unsigned>>{{50, 50}})}};
    add_talents(tier3);

    QMap<QString, Talent*> tier4 {{"4LL", new Talent(pchar, this, "Improved Kick", "4LL", "Assets/ability/Ability_kick.png", 2, "Gives your Kick ability a %1% chance to silence the target for 2 sec.", QVector<QPair<unsigned, unsigned>>{{50, 50}})},
                                  {"4ML", new DaggerSpecialization(pchar, this)},
                                  {"4MR", get_dual_wield_spec()}};
    add_talents(tier4);

    QMap<QString, Talent*> tier5 {{"5LL", new MaceSpecialization(pchar, this)},
                                  {"5ML", get_blade_flurry()},
                                  {"5MR", get_sword_spec()},
                                  {"5RR", new FistWeaponSpecialization(pchar, this)}};
    add_talents(tier5);

    QMap<QString, Talent*> tier6 {{"6ML", new WeaponExpertise(pchar, this)},
                                  {"6MR", get_aggression()}};
    add_talents(tier6);

    QMap<QString, Talent*> tier7 {{"7ML", get_adrenaline_rush()}};
    add_talents(tier7);

    talents["2ML"]->talent->set_bottom_child(talents["3ML"]->talent);
    talents["3ML"]->talent->set_parent(talents["2ML"]->talent);

    talents["2MR"]->talent->set_bottom_child(talents["4MR"]->talent);
    talents["4MR"]->talent->set_parent(talents["2MR"]->talent);

    talents["5ML"]->talent->set_bottom_child(talents["6ML"]->talent);
    talents["6ML"]->talent->set_parent(talents["5ML"]->talent);
}

Talent* Combat::get_improved_sinister_strike() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Reduces the Energy cost of your Sinister Strike ability by %1.";
    Talent::initialize_rank_descriptions(rank_descriptions, base_str, 2, QVector<QPair<unsigned, unsigned>>{{3, 2}});
    Talent* talent = new Talent(rogue, this, "Improved Sinister Strike", "1ML",
                                "Assets/spell/Spell_shadow_ritualofsacrifice.png", 2, rank_descriptions,
                                QVector<SpellRankGroup*>{spells->get_spell_rank_group_by_name("Sinister Strike")});

    return talent;
}

Talent* Combat::get_improved_backstab() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Increases the critical strike chance of your Backstab ability by %1%.";
    Talent::initialize_rank_descriptions(rank_descriptions, base_str, 3, QVector<QPair<unsigned, unsigned>>{{10, 10}});
    Talent* talent = new Talent(rogue, this, "Improved Backstab", "2LL",
                                "Assets/ability/Ability_backstab.png", 3, rank_descriptions,
                                QVector<SpellRankGroup*>{spells->get_spell_rank_group_by_name("Backstab")});

    return talent;
}

Talent* Combat::get_dual_wield_spec() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Increases the damage done by your offhand weapon by %1%.";
    Talent::initialize_rank_descriptions(rank_descriptions, base_str, 5, QVector<QPair<unsigned, unsigned>>{{10, 10}});
    Talent* talent = new Talent(rogue, this, "Dual Wield Specialization", "4MR",
                                "Assets/ability/Ability_dualwield.png", 5, rank_descriptions,
                                QVector<SpellRankGroup*>{spells->get_spell_rank_group_by_name("Offhand Attack")});

    return talent;
}

Talent* Combat::get_blade_flurry() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Increases your attack speed by 20%. In addition, attacks strike an additional nearby opponent. Lasts 15 sec.";
    rank_descriptions.insert(0, base_str);
    rank_descriptions.insert(1, base_str);
    Talent* talent = new Talent(rogue, this, "Blade Flurry", "5ML",
                                "Assets/ability/Ability_warrior_punishingblow.png", 1, rank_descriptions,
                                QVector<SpellRankGroup*>{spells->get_spell_rank_group_by_name("Blade Flurry")});

    return talent;
}

Talent* Combat::get_sword_spec() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Gives you a %1% chance to get an extra attack on the same target after dealing damage with your Sword.";
    Talent::initialize_rank_descriptions(rank_descriptions, base_str, 5, QVector<QPair<unsigned, unsigned>>{{1, 1}});
    Talent* talent = new Talent(rogue, this, "Sword Specialization", "5MR",
                                "Assets/items/Inv_sword_27.png", 5, rank_descriptions,
                                {}, {},
                                QVector<Proc*>{rogue->get_sword_spec()});

    return talent;
}

Talent* Combat::get_aggression() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Increases the damage of your Sinister Strike and Eviscerate abilities by %1%.";
    Talent::initialize_rank_descriptions(rank_descriptions, base_str, 3, QVector<QPair<unsigned, unsigned>>{{2, 2}});
    Talent* talent = new Talent(rogue, this, "Aggression", "6MR",
                                "Assets/ability/Ability_racial_avatar.png", 3, rank_descriptions,
                                QVector<SpellRankGroup*>{
                                    spells->get_spell_rank_group_by_name("Eviscerate"),
                                    spells->get_spell_rank_group_by_name("Sinister Strike")});

    return talent;
}

Talent* Combat::get_adrenaline_rush() {
    QMap<unsigned, QString> rank_descriptions;
    QString base_str = "Increases your Energy regeneration rate by 100% for 15 sec.";
    rank_descriptions.insert(0, base_str);
    rank_descriptions.insert(1, base_str);
    Talent* talent = new Talent(rogue, this, "Adrenaline Rush", "7ML",
                                "Assets/spell/Spell_shadow_shadowworddominate.png", 1, rank_descriptions,
                                QVector<SpellRankGroup*>{spells->get_spell_rank_group_by_name("Adrenaline Rush")});

    return talent;
}

Talent* Combat::get_endurance() {
    QString base_str = "Reduces the cooldown of your Sprint and Evasion abilities by %1.";
    QMap<unsigned, QString> rank_descriptions {{0, base_str.arg("45 sec")}, {1, base_str.arg("45 sec")},
                                               {2, base_str.arg("1.5 min")}};
    Talent* talent = new Talent(rogue, this, "Endurance", "3LL", "Assets/spell/Spell_shadow_shadowward.png", 2,
                                rank_descriptions);

    return talent;
}
