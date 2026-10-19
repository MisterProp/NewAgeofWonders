// unit_test.cpp - rule-fidelity tests for the reconstructed Unit class.
//
// Verifies the recovered stat/rank/morale maths and (on a 32-bit build) the
// exact object layout against the AoWEPACK.dpl decompilation. Usage:
//
//   aow_unit_tests        (no data files needed)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

#include "aow/unit.hpp"

// The Unit objects are intentionally non-standard-layout (vtable first), so
// offsetof() needs the warning suppressed.
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace {

int g_fail = 0;
int g_pass = 0;

void check(bool ok, const std::string& what) {
    if (ok) {
        ++g_pass;
    } else {
        ++g_fail;
        std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    }
}

// A representative resource: "Human Archer"-like defaults (Create @0x55784c44
// uses moves=16, hp=5, attack/defense/damage/resistance=3) with level 2.
aow::unit::UnitResource make_resource() {
    aow::unit::UnitResource r{};
    r.attack = 3;
    r.defense = 3;
    r.damage = 3;
    r.hit_points = 5;
    r.moves = 16;
    r.resistance = 3;
    r.level = 2;
    r.unit_size = 1;
    r.blood_type = 1;
    r.alignment = 3;
    return r;
}

}  // namespace

int main() {
    using namespace aow::unit;

    // --- Morale mapping (MoraleValueToMorale / MoraleToMoraleValue) ---------
    check(morale_from_value(0) == Morale::Broken, "value 0 -> Broken");
    check(morale_from_value(20) == Morale::Broken, "value 20 -> Broken");
    check(morale_from_value(21) == Morale::Low, "value 21 -> Low");
    check(morale_from_value(40) == Morale::Low, "value 40 -> Low");
    check(morale_from_value(41) == Morale::Normal, "value 41 -> Normal");
    check(morale_from_value(60) == Morale::Normal, "value 60 -> Normal");
    check(morale_from_value(61) == Morale::High, "value 61 -> High");
    check(morale_from_value(80) == Morale::High, "value 80 -> High");
    check(morale_from_value(81) == Morale::Fanatic, "value 81 -> Fanatic");
    check(morale_value(Morale::Broken) == 10, "Broken value == 10");
    check(morale_value(Morale::Normal) == 50, "Normal value == 50");
    check(morale_value(Morale::Fanatic) == 90, "Fanatic value == 90");

    UnitResource res = make_resource();
    Unit u{};
    u.resource = &res;
    u.flags = kUnitFlag_strengthValid;
    u.morale_value = 50;  // Normal morale => 0 defense/resistance modifier

    // --- Rank thresholds: level 2 => 4 (seasoned) and 12 (veteran) ----------
    u.experience = 3;
    check(u.rank() == Rank::Recruit, "exp 3 / lvl 2 -> Recruit");
    check(u.next_level_experience() == 4, "next level exp == lvl*2 == 4");
    u.experience = 4;
    check(u.rank() == Rank::Seasoned, "exp 4 / lvl 2 -> Seasoned");
    check(u.next_level_experience() == 12, "next level exp == lvl*6 == 12");
    u.experience = 11;
    check(u.rank() == Rank::Seasoned, "exp 11 -> Seasoned");
    u.experience = 12;
    check(u.rank() == Rank::Veteran, "exp 12 -> Veteran");
    check(u.next_level_experience() == 0, "maxed -> next exp 0");

    // --- Stat getters at each rank (progression: atk/def 0,1,1; dmg/res 0,0,1;
    //     hits 0,1,2) --------------------------------------------------------
    u.experience = 0;  // Recruit
    check(u.attack() == 3, "Recruit attack == 3");
    check(u.defense() == 3, "Recruit defense == 3");
    check(u.damage() == 3, "Recruit damage == 3");
    check(u.resistance() == 3, "Recruit resistance == 3");
    check(u.hits() == 5, "Recruit hits == 5");
    check(u.moves() == 16, "moves == 16 (resource)");
    check(u.unit_level() == 2, "unit_level == 2");
    check(u.upkeep() == 6, "upkeep == (lvl+1)*2 == 6");

    u.experience = 4;  // Seasoned
    check(u.attack() == 4, "Seasoned attack == 4");
    check(u.defense() == 4, "Seasoned defense == 4");
    check(u.damage() == 3, "Seasoned damage == 3");
    check(u.resistance() == 3, "Seasoned resistance == 3");
    check(u.hits() == 6, "Seasoned hits == 6");

    u.experience = 12;  // Veteran
    check(u.attack() == 4, "Veteran attack == 4");
    check(u.damage() == 4, "Veteran damage == 4");
    check(u.resistance() == 4, "Veteran resistance == 4");
    check(u.hits() == 7, "Veteran hits == 7");

    // --- Inherent getters ignore the instance bonus -------------------------
    u.experience = 0;
    u.recompute_bonuses(5, 5, 5, 5);
    check(u.inherent_attack() == 3, "inherent attack ignores bonus");
    check(u.attack() == 8, "attack adds bonus -> 8");
    u.recompute_bonuses(20, 0, 0, 0);
    check(u.attack() == 10, "attack clamps to 10");
    u.recompute_bonuses(-20, 0, 0, 0);
    check(u.attack() == 1, "attack clamps to 1");

    // --- Morale modifiers on defense/resistance -----------------------------
    u.recompute_bonuses(0, 0, 0, 0);
    u.morale_value = 10;  // Broken
    check(u.morale() == Morale::Broken, "morale value 10 -> Broken");
    check(u.defense() == 1, "Broken morale: defense 3-2 -> 1");
    u.morale_value = 50;  // Normal
    check(u.defense() == 3, "Normal morale: defense == 3");

    // --- Hit/move point clamping -------------------------------------------
    u.set_hit_points(100);
    check(u.hit_points == 5, "set_hit_points clamps to hits()");
    u.set_hit_points(-4);
    check(u.hit_points == 0, "set_hit_points clamps to 0");
    u.set_move_points(100);
    check(u.move_points == 16, "set_move_points clamps to moves()");

    // --- SetRank / SetExperience -------------------------------------------
    u.experience = 0;
    u.set_rank(Rank::Seasoned);
    check(u.experience == 4, "set_rank(Seasoned) -> exp 4");
    u.set_rank(Rank::Veteran);
    check(u.experience == 12, "set_rank(Veteran) -> exp 12");
    u.set_rank(Rank::Recruit);
    check(u.experience == 0, "set_rank(Recruit) -> exp 0");

    // --- Ability bit array --------------------------------------------------
    std::uint8_t bits[16] = {0};  // >= ceil(0x6c+1 / 8) = 14 bytes
    u.bits = bits;
    u.count = 30;
    u.set(0, true);
    u.set(13, true);
    check(u.get(0) && u.get(13), "ability bits 0 and 13 set");
    check(!u.get(1) && !u.get(29), "ability bits 1 and 29 clear");
    check(!u.get(30), "out-of-range ability -> false");
    u.set(0, false);
    check(!u.get(0), "ability bit cleared");
    check(u.alignment() == res.alignment, "alignment == record when no 0x6b");
    u.count = 0x6c + 1;
    u.set(0x6b, true);
    check(u.alignment() == 5, "alignment forced to 5 by ability 0x6b");

    // --- Strength system ----------------------------------------------------
    u.strength_mul = 11;
    u.strength[2] = 4;
    u.strength_all = 7;
    check(u.strength_of(2) == (4 + 10) * 11, "strength_of(2)");
    check(u.strength_of(3) == 0, "strength_of(unset) == 0");
    check(u.strength_max(kDamageTypeAll) == (7 + 10) * 11, "strength_max(dtAll)");

    // --- Layout (enforced in a 32-bit build) --------------------------------
    if (sizeof(void*) == 4) {
        check(sizeof(AbilityList) == 0x18, "sizeof(AbilityList) == 0x18");
        check(sizeof(AbstractUnit) == 0x3c, "sizeof(AbstractUnit) == 0x3c");
        check(sizeof(Unit) == 0x48, "sizeof(Unit) == 0x48");
        check(sizeof(UnitResource) == 0x54, "sizeof(UnitResource) == 0x54");
        check(offsetof(AbstractUnit, strength_mul) == 0x34, "strength_mul @0x34");
        check(offsetof(Unit, resource) == 0x40, "resource @0x40");
    }

    std::printf("unit: %d checks passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
