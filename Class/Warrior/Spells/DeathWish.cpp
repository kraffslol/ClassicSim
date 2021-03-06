#include "DeathWish.h"

#include "DeathWishBuff.h"
#include "CooldownControl.h"
#include "Warrior.h"

DeathWish::DeathWish(Character* pchar) :
    Spell("Death Wish", "Assets/spell/Spell_shadow_deathpact.png", pchar, new CooldownControl(pchar, 180.0), RestrictedByGcd::Yes, ResourceType::Rage, 10),
    TalentRequirer(QVector<TalentRequirerInfo*>{new TalentRequirerInfo("Death Wish", 1, DisabledAtZero::Yes)}),
    warr(dynamic_cast<Warrior*>(pchar)),
    death_wish_buff(new DeathWishBuff(pchar))
{
    this->enabled = false;
}

DeathWish::~DeathWish() {
    delete death_wish_buff;
    delete cooldown;
}

void DeathWish::spell_effect() {
    death_wish_buff->apply_buff();

    cooldown->add_spell_cd_event();
    cooldown->add_gcd_event();

    warr->lose_rage(resource_cost);
}

SpellStatus DeathWish::is_ready_spell_specific() const {
    if (warr->in_defensive_stance())
        return SpellStatus::InDefensiveStance;

    return warr->on_stance_cooldown() ? SpellStatus::OnStanceCooldown : SpellStatus::Available;
}

void DeathWish::increase_talent_rank_effect(const QString&, const int) {
    death_wish_buff->enable_buff();
}

void DeathWish::decrease_talent_rank_effect(const QString&, const int) {
    death_wish_buff->disable_buff();
}
