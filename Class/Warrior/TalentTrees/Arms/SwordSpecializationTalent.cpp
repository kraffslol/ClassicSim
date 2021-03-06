#include "SwordSpecializationTalent.h"

#include "SwordSpecialization.h"
#include "Warrior.h"
#include "WarriorSpells.h"

SwordSpecializationTalent::SwordSpecializationTalent(Warrior* pchar, TalentTree* tree) :
    Talent(pchar, tree, "Sword Specialization", "5RR", "Assets/items/Inv_sword_27.png", 5),
    sword_spec(dynamic_cast<SwordSpecialization*>(dynamic_cast<WarriorSpells*>(pchar->get_spells())->get_sword_spec()))
{
    QString base_str = "Gives you a %1% chance to get an extra attack on the same target after dealing damage with your Sword.";
    initialize_rank_descriptions(rank_descriptions, base_str, max_points, QVector<QPair<unsigned, unsigned>>{{1, 1}});
}

SwordSpecializationTalent::~SwordSpecializationTalent() = default;

void SwordSpecializationTalent::apply_rank_effect() {
    sword_spec->increase_talent_rank(sword_spec, name);
}

void SwordSpecializationTalent::remove_rank_effect() {
    sword_spec->decrease_talent_rank(sword_spec, name);
}
