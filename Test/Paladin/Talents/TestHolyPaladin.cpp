#include "TestHolyPaladin.h"

#include "Human.h"
#include "Paladin.h"

TestHolyPaladin::TestHolyPaladin(EquipmentDb* equipment_db):
    TestTalentTree(equipment_db, "LEFT")
{}

void TestHolyPaladin::set_up() {
    race = new Human();
    pchar = new Paladin(race, equipment_db, nullptr);
}

void TestHolyPaladin::tear_down() {
    delete pchar;
    delete race;
}

void TestHolyPaladin::test_spending_talent_points() {
    assert(!decrement("Divine Intellect"));

    assert(increment("Divine Intellect", 5));
    // 5 points
    assert(!increment("Divine Intellect"));
    assert(decrement("Divine Intellect"));
    assert(increment("Divine Intellect"));

    // Spend point in T2 and check that T1 cannot be decremented below a total of 5 points.
    assert(increment("Spiritual Focus"));
    assert(!decrement("Divine Intellect"));
    assert(increment("Divine Strength"));
    assert(decrement("Divine Intellect"));
    assert(!decrement("Divine Intellect"));
    assert(!decrement("Divine Strength"));

    assert(increment("Divine Intellect"));
    assert(decrement("Divine Strength"));
    assert(increment("Spiritual Focus", 4));

    // Assert cannot spend points into Divine Favor if 5/5 Illumination is missing
    assert(increment("Improyed Lay on Hands", 2));
    assert(increment("Healing Light", 3));
    assert(increment("Consecration", 1));
    assert(increment("Unyielding Faith", 2));
    assert(increment("Improved Blessing of Wisdom", 2));
    assert(tree_has_points(20));

    assert(!increment("Divine Favor"));
    assert(increment("Illumination"));
    assert(!increment("Divine Favor"));
    assert(increment("Illumination"));
    assert(!increment("Divine Favor"));
    assert(increment("Illumination"));
    assert(!increment("Divine Favor"));
    assert(increment("Illumination"));
    assert(!increment("Divine Favor"));
    assert(increment("Illumination"));
    assert(increment("Divine Favor"));
    assert(tree_has_points(26));

    // Assert cannot remove points from Illumination once points are spent in Holy Power
    assert(!decrement("Illumination"));
    assert(decrement("Divine Favor"));
    assert(decrement("Illumination"));

    // Assert cannot increment Holy Shock without Divine Favor
    assert(increment("Illumination"));
    assert(increment("Holy Power", 5));
    assert(tree_has_points(30));

    assert(!increment("Holy Shock"));
    assert(increment("Divine Favor"));
    assert(increment("Holy Shock"));

    assert(tree_has_points(32));
    // Assert cannot decrement lower tiers
    assert(!decrement("Divine Intellect"));
    assert(!decrement("Spiritual Focus"));
    assert(decrement("Improyed Lay on Hands"));
    assert(!decrement("Improyed Lay on Hands"));
    assert(!decrement("Improved Blessing of Wisdom"));
    assert(!decrement("Divine Favor"));
    assert(!decrement("Holy Power"));
}

void TestHolyPaladin::test_clearing_tree_after_filling() {
    spec_holy();

    clear_tree();

    assert(tree_has_points(0));
}

void TestHolyPaladin::test_refilling_tree_after_switching_talent_setup() {
    assert(tree_has_points(0));
    spec_holy();
    assert(tree_has_points(45));

    switch_to_setup(1);
    assert(tree_has_points(0));
    spec_holy();
    assert(tree_has_points(45));

    switch_to_setup(2);
    assert(tree_has_points(0));
    spec_holy();
    assert(tree_has_points(45));
}

void TestHolyPaladin::spec_holy() {
    assert(increment("Divine Strength", 5));
    assert(increment("Divine Intellect", 5));
    assert(increment("Spiritual Focus", 5));
    assert(increment("Improved Seal of Righteousness", 5));
    assert(increment("Healing Light", 3));
    assert(increment("Consecration", 1));
    assert(increment("Improyed Lay on Hands", 2));
    assert(increment("Unyielding Faith", 2));
    assert(increment("Illumination", 5));
    assert(increment("Improved Blessing of Wisdom", 2));
    assert(increment("Divine Favor", 1));
    assert(increment("Lasting Judgement", 3));
    assert(increment("Holy Power", 5));
    assert(increment("Holy Shock", 1));
}

QString TestHolyPaladin::get_position(const QString& name) const {
    if (name == "Divine Strength")
        return "1ML";
    if (name == "Divine Intellect")
        return "1MR";
    if (name == "Spiritual Focus")
        return "2ML";
    if (name == "Improved Seal of Righteousness")
        return "2MR";
    if (name == "Healing Light")
        return "3LL";
    if (name == "Consecration")
        return "3ML";
    if (name == "Improyed Lay on Hands")
        return "3MR";
    if (name == "Unyielding Faith")
        return "3RR";
    if (name == "Illumination")
        return "4ML";
    if (name == "Improved Blessing of Wisdom")
        return "4MR";
    if (name == "Divine Favor")
        return "5ML";
    if (name == "Lasting Judgement")
        return "5MR";
    if (name == "Holy Power")
        return "6MR";
    if (name == "Holy Shock")
        return "7ML";

    return "";
}
