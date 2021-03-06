#include "RogueSpells.h"

#include "AdrenalineRush.h"
#include "Backstab.h"
#include "BladeFlurry.h"
#include "Engine.h"
#include "Eviscerate.h"
#include "Hemorrhage.h"
#include "MainhandAttack.h"
#include "OffhandAttackRogue.h"
#include "OffhandMeleeHit.h"
#include "Rogue.h"
#include "SinisterStrike.h"
#include "SliceAndDice.h"

RogueSpells::RogueSpells(Rogue* rogue) :
    CharacterSpells(rogue),
    rogue(rogue)
{
    this->mh_attack = new MainhandAttack(rogue);
    this->oh_attack = new OffhandAttackRogue(rogue);
    this->adrenaline_rush = new AdrenalineRush(rogue);
    this->blade_flurry = new BladeFlurry(rogue);
    this->backstab = new Backstab(rogue);
    this->eviscerate = new Eviscerate(rogue);
    this->hemorrhage = new Hemorrhage(rogue);
    this->sinister_strike = new SinisterStrike(rogue);
    this->slice_and_dice = new SliceAndDice(rogue);

    add_spell_group({adrenaline_rush});
    add_spell_group({backstab});
    add_spell_group({blade_flurry});
    add_spell_group({eviscerate});
    add_spell_group({hemorrhage});
    add_spell_group({mh_attack});
    add_spell_group({oh_attack});
    add_spell_group({sinister_strike});
    add_spell_group({slice_and_dice});
}

RogueSpells::~RogueSpells() = default;

void RogueSpells::oh_auto_attack(const int iteration) {
    if (!oh_attack->attack_is_valid(iteration))
        return;

    if (!is_melee_attacking())
        return;

    oh_attack->perform();

    add_next_oh_attack();
}

void RogueSpells::add_next_oh_attack() {
    auto* new_event = new OffhandMeleeHit(this, oh_attack->get_next_expected_use(), oh_attack->get_next_iteration());
    rogue->get_engine()->add_event(new_event);
}

OffhandAttack* RogueSpells::get_oh_attack() const {
    return oh_attack;
}

AdrenalineRush* RogueSpells::get_adrenaline_rush() const {
    return this->adrenaline_rush;
}

Backstab* RogueSpells::get_backstab() const {
    return this->backstab;
}

BladeFlurry* RogueSpells::get_blade_flurry() const {
    return this->blade_flurry;
}

Eviscerate* RogueSpells::get_eviscerate() const {
    return this->eviscerate;
}

Hemorrhage* RogueSpells::get_hemorrhage() const {
    return this->hemorrhage;
}

SinisterStrike* RogueSpells::get_sinister_strike() const {
    return this->sinister_strike;
}

SliceAndDice* RogueSpells::get_slice_and_dice() const {
    return this->slice_and_dice;
}
