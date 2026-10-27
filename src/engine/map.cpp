// map.cpp - loader for the AoW1 scenario/campaign map format (Stage 2.1).
//
// See map_types.hpp for the format + the decompiled evidence. The reader is
// stdlib-only and reuses the same property-table walk as pfs.cpp (the container
// is the generic Engine EObject graph).
#include "aow/map_types.hpp"

#include <fstream>
#include <iterator>
#include <utility>

#include "aow/endian.hpp"
#include "aow/eobject.hpp"
#include "aow/inflate.hpp"

namespace aow {
namespace {

// The property-table walk is shared with the resource loader (aow/eobject.hpp).
using Table = EObjectTable;
using Entry = EObjectEntry;

bool read_table(const std::uint8_t* R, std::size_t n, std::size_t pos, Table& out) {
    return read_eobject_table(R, n, pos, out);
}

std::size_t find_off(const Table& t, std::uint32_t tag) {
    return eobject_find(t, tag);
}

std::string read_short_string(const std::uint8_t* R, std::size_t n, std::size_t off) {
    return eobject_short_string(R, n, off);
}

// Engine class ids of the production objects (from their ClassID getters,
// re/exports/aowepack/AoWE.TUnitProduction.ClassID.c and the class table).
constexpr std::uint32_t kUnitProductionClassId = 0x20299;  // AoWE.TUnitProduction
constexpr std::uint32_t kProductionItemClassId = 0x20297;  // AoWE.TProductionItem

// Reads a TProductionItem (+8 kind / +0xc index / +0x10 extra) from the rw
// table `t` whose base is relative to the object's own property table.
CityQueueItem read_production_item(const std::uint8_t* R, std::size_t n,
                                   const Table& t) {
    CityQueueItem it;
    const std::size_t k = find_off(t, 0x14);
    if (k != static_cast<std::size_t>(-1) && t.base + k + 4 <= n) {
        it.kind = read_i32(R + t.base + k);
    }
    const std::size_t i = find_off(t, 0x15);
    if (i != static_cast<std::size_t>(-1) && t.base + i + 4 <= n) {
        it.index = read_i32(R + t.base + i);
    }
    const std::size_t e = find_off(t, 0x16);
    if (e != static_cast<std::size_t>(-1) && t.base + e + 4 <= n) {
        it.extra = read_i32(R + t.base + e);
    }
    return it;
}

// Parses City.TCity rw tag 0x12 (`AoWE.TProductionControl`) into `out`.
//
// `TCity.ReadWrite@557acbcc` serialises the control with `rwEObject`, so the
// tag points at the control's own property table (no class id). The control's
// two lists are `TNode`s: their table tag 1 is a children table whose entries
// are offsets to `rwCreateEObject` records (class id + rw table). This is the
// same shape the TMapField parser uses for placed sprites.
void parse_production(const std::uint8_t* R, std::size_t n, const Table& city,
                      MapHs& out) {
    const std::size_t pc_off = find_off(city, 0x12);
    if (pc_off == static_cast<std::size_t>(-1)) return;
    Table pc;
    if (!read_table(R, n, city.base + pc_off, pc)) return;

    // TProductionControl+0x20, rw tag 0x17: the production state byte. 2 means
    // a production item is active; `TCity.GetActiveAction@557a96a8` only reads
    // the queue when this byte is 2.
    const std::size_t state = find_off(pc, 0x17);
    if (state != static_cast<std::size_t>(-1) && pc.base + state < n) {
        out.production_state = R[pc.base + state];
    }
    // TProductionControl+0x18, rw tag 0x18: LoopQueueBox.Checked.
    const std::size_t loop = find_off(pc, 0x18);
    if (loop != static_cast<std::size_t>(-1) && pc.base + loop < n) {
        out.loop_queue = R[pc.base + loop] != 0;
    }

    // TProductionControl+0x24 (rw tag 0x16) is the TProductionSettings object
    // `ExecuteProduction@5575d118` fills while an item runs. It is NOT a
    // TProductionItem: `TProductionSettings.ReadWrite@5575c8c4` streams
    //   tag 0x0a -> +8    paid gold    (rwInteger)
    //   tag 0x0b -> +0xc  paid mana    (rwInteger)
    //   tag 0x0c -> +0x10 turns left   (rwByte, read as a signed char)
    //   tag 0x0d -> +0x1c total turns  (rwByte)
    //   tag 0x0e -> +0x1d owner player (rwByte)
    //   tag 0x0f -> +0x20 group production id
    //   tag 0x10 -> +0x28 run id
    //   tag 0x11 -> +0x24 produced-unit object
    //   tag 0x12 -> +0x14, tag 0x13 -> +0x18 (production upkeep/group data)
    // `TProductionControl.NewTurn@5575dd00` decrements +0x10 and
    // `FUN_0043f574` formats that same byte as the overview's "N turns left",
    // so a save loaded mid-production must carry these into the runtime fields
    // (the queue rows are the ones with tags 0x14/0x15/0x16).
    const std::size_t cur = find_off(pc, 0x16);
    if (cur != static_cast<std::size_t>(-1)) {
        Table ct;
        if (read_table(R, n, pc.base + cur, ct)) {
            out.production_current_present = true;
            auto read_i32_field = [&](std::uint32_t tag, std::int32_t& dst) {
                const std::size_t o = find_off(ct, tag);
                if (o != static_cast<std::size_t>(-1) && ct.base + o + 4 <= n) {
                    dst = read_i32(R + ct.base + o);
                }
            };
            auto read_i8_field = [&](std::uint32_t tag, std::int32_t& dst) {
                const std::size_t o = find_off(ct, tag);
                if (o != static_cast<std::size_t>(-1) && ct.base + o < n) {
                    dst = static_cast<std::int8_t>(R[ct.base + o]);
                }
            };
            read_i32_field(0x0a, out.production_gold_paid);
            read_i32_field(0x0b, out.production_mana_paid);
            read_i8_field(0x0c, out.production_turns_left);
            read_i8_field(0x0d, out.production_turns_total);
            // The runtime mirror keeps the TProductionItem shape used by the
            // rest of MapHs (kind/index/extra = paid gold/mana/turns left).
            out.production_current.kind = out.production_gold_paid;
            out.production_current.index = out.production_mana_paid;
            out.production_current.extra = out.production_turns_left;
        }
    }

    auto read_children = [&](std::uint32_t tag, Table& out_children) -> bool {
        const std::size_t obj = find_off(pc, tag);
        if (obj == static_cast<std::size_t>(-1)) return false;
        Table obj_t;
        if (!read_table(R, n, pc.base + obj, obj_t)) return false;
        const std::size_t list = find_off(obj_t, 1);
        if (list == static_cast<std::size_t>(-1)) return false;
        return read_table(R, n, obj_t.base + list, out_children);
    };

    // tag 0x15: TUnitProductionList -> TUnitProduction children.
    Table units;
    if (read_children(0x15, units)) {
        for (const Entry& e : units.entries) {
            const std::size_t a = units.base + e.second;
            if (a + 4 > n || read_u32(R + a) != kUnitProductionClassId) {
                continue;
            }
            Table ut;
            if (!read_table(R, n, a + 4, ut)) continue;
            CityProductionUnit u;
            const std::size_t id = find_off(ut, 0x14);
            if (id != static_cast<std::size_t>(-1) && ut.base + id + 4 <= n) {
                u.unit_id = read_i32(R + ut.base + id);
            }
            // rw tag 0x15 is a rwBool: its mere presence sets +0xc.
            u.flag = find_off(ut, 0x15) != static_cast<std::size_t>(-1);
            out.production_units.push_back(u);
        }
    }

    // tag 0x14: TProductionQueue -> TProductionItem children.
    Table queue;
    if (read_children(0x14, queue)) {
        for (const Entry& e : queue.entries) {
            const std::size_t a = queue.base + e.second;
            if (a + 4 > n || read_u32(R + a) != kProductionItemClassId) {
                continue;
            }
            Table it;
            if (!read_table(R, n, a + 4, it)) continue;
            out.production_queue.push_back(read_production_item(R, n, it));
        }
    }
}

// Parses the army `hs` (an AoWE.TArmyHS). Its rw tag 10 is the embedded
// `AoWE.TArmy` object (rwEObject = a bare property table, no class id); the
// army's children (rw tag 1) are the unit objects. Fills out.units (lead first)
// and out.army_direction (the TUnitHS rw tag 9 facing byte). Evidence:
// re/exports/aowepack/AoWE.TArmyHS.ReadWrite.c (chain TUnitHS.ReadWrite tag 9 +
// tag 10), AoWE.TUnitHS.ReadWrite.c, AoWE.TArmy.ReadWrite.c (children tag 1),
// AoWE.TArmy.UpdateUnitGFXResource.c (the lead unit's gfx is drawn).
void parse_army(const std::uint8_t* R, std::size_t n, const Table& hs,
                MapHs& out) {
    const std::size_t d9 = find_off(hs, 9);
    if (d9 != static_cast<std::size_t>(-1) && hs.base + d9 < n) {
        out.army_direction = R[hs.base + d9];
    }
    const std::size_t t10 = find_off(hs, 10);
    if (t10 == static_cast<std::size_t>(-1)) return;
    Table army;
    if (!read_table(R, n, hs.base + t10, army)) return;
    // The army's owner player is TArmy rw tag 4 (`TArmy.GetPlayer` @ 0x5578d504
    // reads +0x12; the stream omits a zero, so a missing tag = owner 0).
    const std::size_t own = find_off(army, 4);
    if (own != static_cast<std::size_t>(-1) && army.base + own < n) {
        out.owner = R[army.base + own];
    }
    const std::size_t c1 = find_off(army, 1);
    if (c1 == static_cast<std::size_t>(-1)) return;
    Table units;
    if (!read_table(R, n, army.base + c1, units)) return;

    for (const Entry& ue : units.entries) {
        const std::size_t ua = units.base + ue.second;
        if (ua + 4 > n) continue;
        const std::uint32_t ucid = read_u32(R + ua);
        if (ucid != kMapUnitClassId && ucid != kMapHeroClassId &&
            ucid != kMapLeaderClassId) {
            continue;
        }
        Table ut;
        if (!read_table(R, n, ua + 4, ut)) continue;
        MapUnit mu;
        mu.class_id = ucid;
        const std::size_t t5 = find_off(ut, 5);  // TAbstractUnit id (rwIntegerEx)
        if (t5 != static_cast<std::size_t>(-1) && ut.base + t5 + 4 <= n) {
            mu.id = read_i32(R + ut.base + t5);
        }
        const std::size_t t6 = find_off(ut, 6);  // TAbstractUnit morale (rwByte)
        if (t6 != static_cast<std::size_t>(-1) && ut.base + t6 < n) {
            mu.morale = R[ut.base + t6];
        }
        // TAbilityOwner rw tags 2 (u16 bit count) + 3 (ceil(count/8) bitmap
        // bytes) - the unit's ability list. See docs/15_MOVEMENT.md.
        const std::size_t t2 = find_off(ut, 2);
        const std::size_t t3 = find_off(ut, 3);
        if (t2 != static_cast<std::size_t>(-1) && ut.base + t2 + 2 <= n) {
            const std::uint32_t cnt = read_u16(R + ut.base + t2);
            mu.ability_count = cnt;
            if (t3 != static_cast<std::size_t>(-1)) {
                const std::size_t bytes =
                    (static_cast<std::size_t>(cnt) + 7) / 8;
                const std::size_t p = ut.base + t3;
                if (bytes > 0 && p + bytes <= n) {
                    mu.ability_mask.assign(R + p, R + p + bytes);
                }
            }
        }
        if (ucid == kMapUnitClassId) {
            // TUnit: rw tag 10 (rwWord) = the TUnitResource record-list tag.
            // The stream omits zero values, so a missing tag 10 means id 0.
            const std::size_t t = find_off(ut, 10);
            if (t != static_cast<std::size_t>(-1) && ut.base + t + 2 <= n) {
                mu.resource = read_u16(R + ut.base + t);
            }
        } else {
            // THero/TLeader. rw tag 8 (rwInteger) = the THeroResource index;
            // THero.Create leaves +0x44 = -1 and the stream omits that default,
            // so a missing tag 8 stays -1 (the engine assigns the hero's race
            // from the scenario setup; the renderer falls back to the first
            // hero resource). See docs/13_UNITS_ON_MAP.md.
            mu.is_hero = true;
            mu.resource = -1;
            const std::size_t t8 = find_off(ut, 8);
            if (t8 != static_cast<std::size_t>(-1) && ut.base + t8 + 4 <= n) {
                mu.resource = read_i32(R + ut.base + t8);
            }
            const std::size_t tn = find_off(ut, 10);  // name
            if (tn != static_cast<std::size_t>(-1)) {
                mu.name = read_short_string(R, n, ut.base + tn);
            }
            const std::size_t tk = find_off(ut, 11);  // nickname
            if (tk != static_cast<std::size_t>(-1)) {
                mu.nickname = read_short_string(R, n, ut.base + tk);
            }
            const std::size_t tf = find_off(ut, 7);   // face index (+0x3c)
            if (tf != static_cast<std::size_t>(-1) && ut.base + tf + 4 <= n) {
                mu.face_index = read_i32(R + ut.base + tf);
            }
            const std::size_t tg = find_off(ut, 15);  // gender (+0x69)
            if (tg != static_cast<std::size_t>(-1) && ut.base + tg < n) {
                mu.gender = R[ut.base + tg];
            }
            const std::size_t te = find_off(ut, 24);  // experience (+0x48)
            if (te != static_cast<std::size_t>(-1) && ut.base + te + 4 <= n) {
                mu.experience = read_u32(R + ut.base + te);
            }
            const std::size_t tu = find_off(ut, 22);  // unit level (+0x4c)
            if (tu != static_cast<std::size_t>(-1) && ut.base + tu < n) {
                mu.unit_level = R[ut.base + tu];
            }
            const std::size_t tb = find_off(ut, 38);  // library name (+0x94)
            if (tb != static_cast<std::size_t>(-1)) {
                mu.library_name = read_short_string(R, n, ut.base + tb);
            }
        }
        out.units.push_back(mu);
    }
}

// Reads a `Sound.TSonglist` (or its `AoWE.TStartupSongList` subclass) that the
// map stores as an EObject. `Sound.TSonglist.ReadWrite` writes the song
// `TStringList` at tag 10, which the storage stream serialises as a u32 byte
// length followed by the list's `Text`: the file names joined by CRLF, with a
// trailing CRLF. Returns the song file names in list order (e.g.
// "SONGS\\CLOUDS.IT"). See docs/09_GAME_SCREEN.md (in-game music).
std::vector<std::string> read_songlist(const std::uint8_t* R, std::size_t n,
                                       std::size_t obj_off) {
    std::vector<std::string> songs;
    Table t;
    if (!read_table(R, n, obj_off, t)) return songs;
    const std::size_t so = find_off(t, 10);
    if (so == static_cast<std::size_t>(-1)) return songs;
    const std::size_t p = t.base + so;
    if (p + 4 > n) return songs;
    std::size_t len = read_u32(R + p);
    const std::size_t q = p + 4;
    if (len > n - q) len = n - q;
    const std::string text(reinterpret_cast<const char*>(R + q), len);
    std::size_t start = 0;
    while (start < text.size()) {
        const std::size_t eol = text.find('\n', start);
        std::size_t end = (eol == std::string::npos) ? text.size() : eol;
        if (end > start && text[end - 1] == '\r') --end;  // strip the CRLF
        if (end > start) songs.push_back(text.substr(start, end - start));
        if (eol == std::string::npos) break;
        start = eol + 1;
    }
    return songs;
}

struct ClassName {
    std::uint32_t id;
    const char* name;
};

// Map-related Engine class ids -> names (from the decompiled ClassID getters;
// re/exports/*/[Class].ClassID.c). See docs/01_FORMATS.md.
const ClassName kClassNames[] = {
    {0x20011, "AoWE.TAoWHSMap"},         {0x20012, "AoWE.TAoWMapField"},
    {0x20013, "AoWE.TAoWMapLevel"},      {0x10101, "HSEngine.TMapField"},
    {0x10102, "HSEngine.TMapLevel"},     {0x10103, "HSEngine.TMapContainer"},
    {0x10154, "HSEngine.TMapObjectResource"},
    {0x10156, "HSEngine.TMapObject"},    {0x20166, "Terrain.TTerrainMO"},
    {0x20164, "Road.TRoad"},             {0x20168, "Mountain.TMountainMO"},
    {0x20152, "Hexagon.THexagon"},       {0x20170, "IsoHex.TUpperIsometricHexagon"},
    {0x220101, "AoWTC.TAoWCombatMapField"},
    {0x220102, "AoWTC.TAoWCombatMapLevel"},
    {0x220100, "AoWTC.TAoWCombatMap"},
    {0x20557, "BorderHx.TBorderHexagon"}, {0x2073d, "AoWHex.TAoWWaterHexagon"},
    {0x2016c, "AoWE.TFrozenWaterHS"},    {0x20217, "AoWE.TArmyHS"},
    {0x20271, "AoWE.TItemHS"},           {0x2077e, "AoWE.TFire"},
    {0x20108, "City.TCity"},             {0x2036a, "Crops.TCrop"},
    {0x2036e, "Farm.TFarm"},             {0x20106, "Pad.TPad"},
    {0x2037a, "Mine.TMine"},             {0x2037e, "Cave.TCave"},
    {0x2043e, "Tower.TTower"},           {0x2087a, "HxBridge.THexagonBridge"},
    {0x2019a, "PowerNode.TPowerNode"},   {0x203ae, "Gems.TGems"},
    {0x20391, "UnHolyG.TUnHolyGround"},  {0x20393, "HolyG.THolyGround"},
    {0x20102, "Sign.TSign"},             {0x20104, "Teleport.TTeleport"},
    {0x204be, "Shipyard.TShipyard"},     {0x2092e, "BldGuild.TBuildersGuild"},
    {0x203be, "ManaCrystals.TManaCrystals"},
    {0x2089e, "WizardsTower.TWizardsTower"},
    {0x2039e, "PoisonP.TPoisonPlant"},   {0x2032e, "MonsterL.TMonsterlair"},
    {0x2039a, "Ruin.TRuin"},             {0x2032a, "Dungeon.TDungeon"},
    {0x2030a, "Crypt.TCrypt"},           {0x2063e, "ReflectingPool.TReflectingPool"},
    {0x20100, "Spring.TSpringOfLife"},   {0x204b3, "WaterNode.TWaterNode"},
    {0x204ae, "DeathNode.TDeathNode"},   {0x204ce, "EarthNode.TEarthNode"},
    {0x203de, "FireNode.TFireNode"},     {0x20371, "LifeNode.TLifeNode"},
    {0x2094e, "AirNode.TAirNode"},       {0x2038e, "Pyramid.TPyramid"},
    {0x21242, "SmokeCloud.TSmokeCloud"}, {0x2123f, "PoisonC.TPoisonCloud"},
    {0x2089a, "MagicSanctuary.TMagicSanctuary"},
    {0x2089c, "Arena.TArena"},           {0x20130, "BloodShrine.TBloodShrine"},
    {0x2034a, "AoWE.TStructure"},        {0x2034c, "AoWE.TPlayerStructure"},
    {0x2009c, "OverlayObject.TOverlayObject"},
    // TMapObjectResource subclasses (the Release.hss resource list; names from
    // the ClassID getters, re/exports/*/[Class].ClassID.c).
    {0x2009d, "OverlayObject.TOverlayObjectResource"},
    {0x20101, "Spring.TSpringOfLifeResource"}, {0x20103, "Sign.TSignResource"},
    {0x20105, "Teleport.TTeleportResource"},   {0x20107, "Pad.TPadResource"},
    {0x20109, "City.TCityResource"},           {0x20131, "BloodShrine.TBloodShrineResource"},
    {0x20153, "Hexagon.THexagonResource"},     {0x20154, "RndHex.TRndHexagonResource"},
    {0x20165, "Road.TRoadResource"},           {0x20167, "Terrain.TTerrainMOR"},
    {0x20169, "Mountain.TMountainMOR"},        {0x2016b, "Mountain.TRaisedMountainResource"},
    {0x20171, "IsoHex.TUpperIsometricHexagonResource"},
    {0x20175, "Bridge.TBridgeResource"},       {0x2019b, "PowerNode.TPowerNodeResource"},
    {0x20234, "AltLife.TAltarOfLifeResource"}, {0x20236, "AltDeath.TAltarOfDeathResource"},
    {0x2023b, "AltFire.TAltarOfFireResource"}, {0x2023e, "AltLight.TAltarOfLightningResource"},
    {0x2024c, "AltIce.TAltarOfIceResource"},   {0x2030b, "Crypt.TCryptResource"},
    {0x2030d, "AltPest.TAltarOfPestilenceResource"},
    {0x2032b, "Dungeon.TDungeonResource"},     {0x2032f, "MonsterL.TMonsterlairResource"},
    {0x2036b, "Crops.TCropResource"},          {0x2036f, "Farm.TFarmResource"},
    {0x20372, "LifeNode.TLifeNodeResource"},   {0x2037b, "Mine.TMineResource"},
    {0x2037f, "Cave.TCaveResource"},           {0x2038f, "Pyramid.TPyramidResource"},
    {0x20392, "UnHolyG.TUnHolyGroundResource"}, {0x20394, "HolyG.THolyGroundResource"},
    {0x2039b, "Ruin.TRuinResource"},           {0x2039f, "PoisonP.TPoisonPlantResource"},
    {0x203af, "Gems.TGemsResource"},           {0x203bf, "ManaCrystals.TManaCrystalsResource"},
    {0x203df, "FireNode.TFireNodeResource"},   {0x2043f, "Tower.TTowerResource"},
    {0x204af, "DeathNode.TDeathNodeResource"}, {0x204b4, "WaterNode.TWaterNodeResource"},
    {0x204bf, "Shipyard.TShipyardResource"},   {0x204cf, "EarthNode.TEarthNodeResource"},
    {0x20558, "BorderHx.TBorderHexagonResource"},
    {0x2063f, "ReflectingPool.TReflectingPoolResource"},
    {0x2073e, "AoWHex.TAoWWaterHexagonResource"},
    {0x2077f, "AoWE.TFireResource"},           {0x2087b, "HxBridge.THexagonBridgeResource"},
    {0x2089b, "MagicSanctuary.TMagicSanctuaryResource"},
    {0x2089d, "Arena.TArenaResource"},         {0x2089f, "WizardsTower.TWizardsTowerResource"},
    {0x2092f, "BldGuild.TBuildersGuildResource"}, {0x2094f, "AirNode.TAirNodeResource"},
    {0x21240, "PoisonC.TPoisonCloudResource"}, {0x21243, "SmokeCloud.TSmokeCloudResource"},
};

// Parses a TMapContainer at `ca` (the offset of its u32 class id) and fills
// out.width/height/level_count + out.levels.
bool load_container(const std::uint8_t* R, std::size_t n, std::size_t ca,
                    Map& out, std::string& error) {
    Table ct;
    if (!read_table(R, n, ca + 4, ct)) return false;
    const std::size_t cb = ct.base;
    const std::size_t w3 = find_off(ct, 3), w4 = find_off(ct, 4), w5 = find_off(ct, 5);
    if (w3 == static_cast<std::size_t>(-1) || w4 == static_cast<std::size_t>(-1)) {
        return false;
    }
    out.width = R[cb + w3];
    out.height = R[cb + w4];
    out.level_count = (w5 != static_cast<std::size_t>(-1)) ? R[cb + w5] : 0;

    const std::size_t l_off = find_off(ct, 1);
    if (l_off == static_cast<std::size_t>(-1)) return true;
    Table lch;
    if (!read_table(R, n, cb + l_off, lch)) return true;

    std::size_t li = 0;
    for (const Entry& le : lch.entries) {
        const std::size_t la = lch.base + le.second;
        if (la + 4 > n) continue;
        const std::uint32_t lcid = read_u32(R + la);
        if (lcid != kMapLevelClassId && lcid != kAoWMapLevelClassId &&
            lcid != kAoWCombatMapLevelClassId) continue;
        Table lt;
        if (!read_table(R, n, la + 4, lt)) continue;
        const std::size_t lb = lt.base;
        const std::size_t lw3 = find_off(lt, 3), lw4 = find_off(lt, 4);
        const std::size_t t6 = find_off(lt, 6), t1 = find_off(lt, 1);
        if (lw3 == static_cast<std::size_t>(-1) || lw4 == static_cast<std::size_t>(-1) ||
            t6 == static_cast<std::size_t>(-1) || t1 == static_cast<std::size_t>(-1)) {
            continue;
        }
        MapLevel level;
        level.index = li++;
        level.width = R[lb + lw3];
        level.height = R[lb + lw4];
        const std::size_t start = lb + t6;
        const std::size_t end = lb + t1;
        std::size_t p = start;
        while (p + 4 <= end) {
            const std::uint32_t len = read_u32(R + p);
            if (len == 0 || p + 4 + len > end) {
                error = "field desync";
                return false;
            }
            const std::size_t body = p + 4;
            const std::size_t rec_end = p + 4 + len;
            Table ft;
            MapTile tile;
            tile.tag = static_cast<std::uint32_t>(level.tiles.size());
            if (read_table(R, n, body, ft)) {
                for (std::size_t fi = 0; fi < ft.entries.size(); ++fi) {
                    const std::uint32_t ftag = ft.entries[fi].first;
                    const std::size_t fa = ft.base + ft.entries[fi].second;
                    const std::size_t fnext = (fi + 1 < ft.entries.size())
                        ? ft.base + ft.entries[fi + 1].second : rec_end;
                    if (ftag == 4) {  // TAoWMapField tag 4: the explored bitmask
                        if (fa + 4 <= n) tile.explored = read_u32(R + fa);
                        continue;
                    }
                    if (fa + 4 > n) continue;
                    MapHs hs;
                    hs.tag = ftag;
                    hs.class_id = read_u32(R + fa);
                    hs.offset = fa;
                    Table sub;
                    if (read_table(R, n, fa + 4, sub)) {
                        const std::size_t r1 = find_off(sub, 1);
                        if (r1 != static_cast<std::size_t>(-1) &&
                            sub.base + r1 + 4 <= n) {
                            hs.resource = read_u32(R + sub.base + r1);
                        }
                        // City.TCity rw tags 0xc (name) / 0xd (race) /
                        // 0xf (wall type) / 0x10 (size). See
                        // re/exports/aowepack/City.TCity.ReadWrite.c and
                        // map_types.hpp. Only TCity carries them.
                        if (hs.class_id == kCityClassId) {
                            // City.TCity rw tag 0xc -> +0x48: the city's name,
                            // the very AnsiString `TCity.GetName@557ac918`
                            // copies out (the Structure floater's
                            // StructNameLbl). `TCity.ReadWrite@557acbcc` streams
                            // +0x48 through **rwString** (stream vtable +0x20),
                            // whose payload is written by
                            // `Engine.TEWriteStorageStream.rwString` ->
                            // FUN_5550f7a8 and read back by FUN_5550f81c: one
                            // length byte (0xff => a u32 length follows), then
                            // that many bytes - `eobject_short_string`, *not* a
                            // u32 length. The writer skips an empty string
                            // entirely and the reader LStrClr()s the field when
                            // the tag is absent, so a city that stores no name
                            // keeps "" (and the original shows a blank label;
                            // `TCity.Create`'s `AoWE.NonameRStr` = "Noname" only
                            // ever applies to a city nobody named yet).
                            const std::size_t r12 = find_off(sub, 0xc);
                            if (r12 != static_cast<std::size_t>(-1)) {
                                hs.city_name =
                                    read_short_string(R, n, sub.base + r12);
                            }
                            const std::size_t r13 = find_off(sub, 0xd);
                            if (r13 != static_cast<std::size_t>(-1) &&
                                sub.base + r13 < n) {
                                hs.race = static_cast<std::int8_t>(R[sub.base + r13]);
                            }
                            const std::size_t r15 = find_off(sub, 0xf);
                            if (r15 != static_cast<std::size_t>(-1) &&
                                sub.base + r15 < n) {
                                hs.wall_type =
                                    static_cast<std::int8_t>(R[sub.base + r15]);
                            }
                            // City.TCity rw tag 0x10 -> +0x4d: the size/level
                            // byte TCity.UpdateFlagID packs into the flag byte.
                            const std::size_t r16 = find_off(sub, 0x10);
                            if (r16 != static_cast<std::size_t>(-1) &&
                                sub.base + r16 < n) {
                                hs.city_level = R[sub.base + r16];
                            }
                            // City.TCity rw tag 0x16 -> +0x44: the status byte
                            // (`TCity.GetRazed` reads bit 1).
                            const std::size_t r22 = find_off(sub, 0x16);
                            if (r22 != static_cast<std::size_t>(-1) &&
                                sub.base + r22 < n) {
                                hs.city_flags = R[sub.base + r22];
                                // `TCity.GetRazed@557ab474` is bit 1 of +0x44.
                                hs.razed = (hs.city_flags & 2) != 0;
                            }
                            // City.TCity rw tag 0x12: the production control
                            // (unit list / queue / loop). See parse_production.
                            parse_production(R, n, sub, hs);
                        }
                        // Gems.TGems.ReadWrite@557c4fdc: rw tag 5 -> +0x1c is
                        // the pile's gold amount (rwInteger) and rw tag 6 ->
                        // +0x20 its size class (a 1-byte blob). `TGems.Create`
                        // defaults them to 0 / 2.
                        if (hs.class_id == kGemsClassId) {
                            const std::size_t g5 = find_off(sub, 5);
                            if (g5 != static_cast<std::size_t>(-1) &&
                                sub.base + g5 + 4 <= n) {
                                hs.gems_gold = read_i32(R + sub.base + g5);
                            }
                            const std::size_t g6 = find_off(sub, 6);
                            if (g6 != static_cast<std::size_t>(-1) &&
                                sub.base + g6 < n) {
                                hs.gems_size = R[sub.base + g6];
                            }
                        }
                        // AoWE.TPlayerStructure rw tag 5 -> +0x30: the owning
                        // player index. Only a TPlayerStructure (or subclass)
                        // stores it; a missing tag is 0 (`rwByte`'s absent-tag
                        // default), i.e. unowned / the independents.
                        if (is_player_structure_class(hs.class_id)) {
                            const std::size_t r5 = find_off(sub, 5);
                            if (r5 != static_cast<std::size_t>(-1) &&
                                sub.base + r5 < n) {
                                hs.structure_owner = R[sub.base + r5];
                            }
                            // Farm.TFarm.ReadWrite@557b2c64 and
                            // Mine.TMine.ReadWrite@557b4690 both store a status
                            // byte at rw tag 0x14; bit 0 is their GetRazed.
                            if (hs.class_id == kFarmClassId ||
                                hs.class_id == kMineClassId ||
                                hs.class_id == kPowerNodeClassId) {
                                const std::size_t r20 = find_off(sub, 0x14);
                                if (r20 != static_cast<std::size_t>(-1) &&
                                    sub.base + r20 < n) {
                                    hs.structure_status = R[sub.base + r20];
                                    hs.razed = (hs.structure_status & 1) != 0;
                                }
                            }
                            // PrdPlace.TProductionPlace.ReadWrite@557bec9c:
                            // rw tag 0x0a -> +0x48, 1 = razed.
                            if (is_production_place_class(hs.class_id)) {
                                const std::size_t r10 = find_off(sub, 0x0a);
                                if (r10 != static_cast<std::size_t>(-1) &&
                                    sub.base + r10 < n) {
                                    hs.structure_status = R[sub.base + r10];
                                    hs.razed = hs.structure_status == 1;
                                }
                            }
                        }
                    }
                    const std::size_t he = (fnext > fa && fnext <= n) ? fnext : n;
                    // AoWE.TArmyHS: parse the army's unit list (docs/13).
                    if (hs.class_id == kMapArmyHsClassId && !sub.entries.empty()) {
                        parse_army(R, n, sub, hs);
                    }
                    hs.payload.assign(R + fa, R + he);
                    tile.objects.push_back(std::move(hs));
                }
            }
            level.tiles.push_back(std::move(tile));
            p = rec_end;
        }
        out.levels.push_back(std::move(level));
    }
    return true;
}

}  // namespace

const char* map_class_name(std::uint32_t class_id) {
    for (const ClassName& c : kClassNames) {
        if (c.id == class_id) return c.name;
    }
    return nullptr;
}

// `AoWE.TPlayerStructure` (0x2034c) and every subclass, read out of AoWEPACK.dpl's
// Delphi VMT parent chain (`vmt-36` holds `parentVmt-76`) and confirmed against
// each class's ClassID getter. See aow/map_types.hpp for the tree.
const std::uint32_t kPlayerStructureClasses[] = {
    0x2034c,  // AoWE.TPlayerStructure
    0x2034e,  // Crops.TPlayerCropStructure
    0x203ba,  // PrdPlace.TProductionPlace
    0x20822,  // RandomNode.TRandomNode
    0x20108,  // City.TCity
    0x2036e,  // Farm.TFarm
    0x204be,  // Shipyard.TShipyard
    0x2092e,  // BldGuild.TBuildersGuild
    0x204b3,  // WaterNode.TWaterNode
    0x204ae,  // DeathNode.TDeathNode
    0x204ce,  // EarthNode.TEarthNode
    0x203de,  // FireNode.TFireNode
    0x20371,  // LifeNode.TLifeNode
    0x2094e,  // AirNode.TAirNode
    0x2037a,  // Mine.TMine
    0x2043e,  // Tower.TTower
    0x2019a,  // PowerNode.TPowerNode
    0x2063e,  // ReflectingPool.TReflectingPool
    0x203f1,  // Altar.TAltar
    0x20233,  // AltLife.TAltarOfLife
    0x20235,  // AltDeath.TAltarOfDeath
    0x2023a,  // AltFire.TAltarOfFire
    0x2023d,  // AltLight.TAltarOfLightning
    0x2024b,  // AltIce.TAltarOfIce
    0x2030c,  // AltPest.TAltarOfPestilence
};

bool is_player_structure_class(std::uint32_t class_id) {
    for (std::uint32_t c : kPlayerStructureClasses) {
        if (c == class_id) return true;
    }
    return false;
}

// `PrdPlace.TProductionPlace` (0x203ba) and its subclasses, from the same VMT
// walk (`vmt-36` parent chain) used for kPlayerStructureClasses.
const std::uint32_t kProductionPlaceClasses[] = {
    0x203ba,  // PrdPlace.TProductionPlace
    0x204be,  // Shipyard.TShipyard
    0x2092e,  // BldGuild.TBuildersGuild
    0x204b3,  // WaterNode.TWaterNode
    0x204ae,  // DeathNode.TDeathNode
    0x204ce,  // EarthNode.TEarthNode
    0x203de,  // FireNode.TFireNode
    0x20371,  // LifeNode.TLifeNode
    0x2094e,  // AirNode.TAirNode
    0x20822,  // RandomNode.TRandomNode
};

bool is_production_place_class(std::uint32_t class_id) {
    for (std::uint32_t c : kProductionPlaceClasses) {
        if (c == class_id) return true;
    }
    return false;
}

bool load_map(const std::string& path, Map& out, std::string& error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    std::vector<std::uint8_t> file((std::istreambuf_iterator<char>(in)),
                                   std::istreambuf_iterator<char>());
    if (file.size() < 16) {
        error = "file too small: " + path;
        return false;
    }

    out = Map();
    std::vector<std::uint8_t> raw;
    const bool cfs = file.size() >= 5 && file[0] == 'C' && file[1] == 'F' &&
                     file[2] == 'S' && file[3] == 0;
    if (cfs) {
        if (!zlib_inflate(file.data() + 5, file.size() - 5, raw)) {
            error = "zlib inflate failed: " + path;
            return false;
        }
        out.compressed = true;
    } else {
        raw.swap(file);
    }
    if (raw.size() < 16) {
        error = "decompressed data too small: " + path;
        return false;
    }

    const std::size_t region_off = read_u32(raw.data());
    if (region_off < 16 || region_off >= raw.size()) {
        error = "bad region offset: " + path;
        return false;
    }
    // HSM/CSM: magic "HSM\0" at [4,8), top class id at [12,16). HSS: name-based.
    const bool hsm = raw[4] == 'H' && raw[5] == 'S' && raw[6] == 'M' && raw[7] == 0;
    if (hsm) {
        out.class_id = read_u32(raw.data() + 12);
    } else if (raw[4] == 0x04 && raw[5] == 'H' && raw[6] == 'S' && raw[7] == 'S') {
        out.class_id = 0;  // HSS (game pack): the object class is not stored
    } else {
        error = "not an AoW map (bad magic): " + path;
        return false;
    }

    out.region.assign(raw.begin() + static_cast<std::ptrdiff_t>(region_off), raw.end());
    const std::uint8_t* R = out.region.data();
    const std::size_t n = out.region.size();

    // Outer table: one entry (tag 0 -> offset 0); the top object follows.
    Table outer;
    if (!read_table(R, n, 0, outer)) {
        error = "cannot read map header table: " + path;
        return false;
    }
    const std::size_t t0 = find_off(outer, 0);
    if (t0 == static_cast<std::size_t>(-1)) {
        error = "map header has no top object: " + path;
        return false;
    }
    Table top;
    if (!read_table(R, n, outer.base + t0, top)) {
        error = "cannot read top object: " + path;
        return false;
    }
    const std::size_t tb = top.base;

    const std::size_t hss_off = find_off(top, 2);
    if (hss_off != static_cast<std::size_t>(-1)) {
        out.hss_file = read_short_string(R, n, tb + hss_off);
    }

    // TAoWHSMap rw tag 0x13 -> +0x174: the day counter. A fresh scenario does
    // not store it (rwInteger's absent-tag default is 0); a saved game does.
    const std::size_t day_off = find_off(top, 0x13);
    if (day_off != static_cast<std::size_t>(-1) && tb + day_off + 4 <= n) {
        out.day = read_i32(R + tb + day_off);
    }
    // TAoWHSMap rw tag 0x1c -> +0x230: the map's own RNG state (`TAoWHSMap.
    // Random@5577827c`). `Gems.TGems.GenerateGemsAmount` draws from it.
    const std::size_t seed_off = find_off(top, 0x1c);
    if (seed_off != static_cast<std::size_t>(-1) && tb + seed_off + 4 <= n) {
        out.rand_seed = read_u32(R + tb + seed_off);
    }

    // The scenario music playlist (see map_types.hpp / docs/09_GAME_SCREEN.md).
    const std::size_t s1d = find_off(top, 0x1d);  // AoWE.TStartupSongList
    if (s1d != static_cast<std::size_t>(-1)) {
        out.songs_startup = read_songlist(R, n, tb + s1d);
    }
    const std::size_t s3c = find_off(top, 0x3c);  // Sound.TSonglist
    if (s3c != static_cast<std::size_t>(-1)) {
        out.songs_list = read_songlist(R, n, tb + s3c);
    }

    // Children (tag 1): the TMapContainer.
    const std::size_t c_off = find_off(top, 1);
    if (c_off == static_cast<std::size_t>(-1)) {
        error = "map has no container child: " + path;
        return false;
    }
    Table ch;
    if (!read_table(R, n, tb + c_off, ch)) {
        error = "cannot read children table: " + path;
        return false;
    }

    bool have_container = false;
    for (const Entry& e : ch.entries) {
        const std::size_t ca = ch.base + e.second;
        if (ca + 4 > n) continue;
        if (read_u32(R + ca) != kMapContainerClassId) continue;
        if (load_container(R, n, ca, out, error)) {
            have_container = true;
        }
        break;
    }
    if (!have_container) {
        error = hsm ? ("no TMapContainer in map: " + path)
                    : ("HSS is the game pack (TAoWWHSSet), not a scenario map: " + path);
        return false;
    }
    return true;
}

}  // namespace aow
