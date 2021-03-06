#include "Nightfall.h"

#include "Character.h"
#include "CharacterStats.h"

Nightfall::Nightfall(Character* pchar):
    Buff(pchar, "Nightfall", "Assets/items/Inv_axe_12.png", 5, 0)
{
    this->debuff = true;
}

void Nightfall::buff_effect_when_applied() {
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Arcane);
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Fire);
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Frost);
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Holy);
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Nature);
    pchar->get_stats()->increase_spell_dmg_mod(15, MagicSchool::Shadow);
}

void Nightfall::buff_effect_when_removed() {
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Arcane);
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Fire);
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Frost);
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Holy);
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Nature);
    pchar->get_stats()->decrease_spell_dmg_mod(15, MagicSchool::Shadow);
}
