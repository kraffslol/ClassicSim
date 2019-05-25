#ifndef EXTERNALBUFF_H
#define EXTERNALBUFF_H

#include "AvailableFactions.h"
#include "Buff.h"

#include <QVersionNumber>

enum ExternalBuffName {
    Annihilator = 0,
    BattleShout,
    BattleSquawk,
    BlessingOfKings,
    BlessingOfMight,
    BlessingOfWisdom,
    BrilliantWizardOil,
    CurseOfRecklessness,
    ElementalSharpeningStone,
    ElixirOfBruteForce,
    ElixirOfGiants,
    ElixirOfTheMongoose,
    FaerieFire,
    FengusFerocity,
    FlaskOfDistilledWisdom,
    FlaskOfSupremePower,
    GreaterArcaneElixir,
    GrilledSquid,
    GroundScorpokAssay,
    JujuMight,
    JujuPower,
    MagebloodPotion,
    ManaSpringTotem,
    MarkOfTheWild,
    ROIDS,
    RallyingCryOfTheDragonslayer,
    SaygesDarkFortuneOfDamage,
    ScrollOfStrengthIV,
    SlipkiksSavvy,
    SmokedDesertDumplings,
    SongflowerSerenade,
    SpiritOfZandalar,
    StrengthOfEarthTotem,
    SunderArmor,
    WinterfallFirewater,
};

class ExternalBuff;

ExternalBuff* get_external_buff_by_name(const ExternalBuffName, Character*);

class ExternalBuff: public Buff {
public:
    ExternalBuff(Character*,
                 const QString& name,
                 const int dur,
                 const int base_charges,
                 ExternalBuffName buff_name,
                 AvailableFactions::Name faction,
                 QString icon,
                 QString description);
    virtual ~ExternalBuff();

    QString get_description() const;
    bool valid_for_faction(AvailableFactions::Name faction) const;
    ExternalBuffName get_enum_value() const;

protected:
    virtual void buff_effect_when_applied();
    virtual void buff_effect_when_removed();

    const ExternalBuffName buff_name;
    AvailableFactions::Name faction;
    const QString description;
};

#endif // EXTERNALBUFF_H
