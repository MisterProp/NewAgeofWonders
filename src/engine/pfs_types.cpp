// pfs_types.cpp - implementation of the typed .PFS rule-table views.
//
// See pfs_types.hpp for the tag tables and the decompiled evidence. The reader
// here is dependency-free (stdlib only), mirroring the png.cpp policy.
#include "aow/pfs_types.hpp"

#include "aow/endian.hpp"

namespace aow {

RecordView::RecordView(const PfsRecord& rec,
                       const std::vector<std::uint8_t>& file)
    : fields_(&rec.fields) {
    if (rec.file_offset < file.size()) {
        rec_ = file.data() + rec.file_offset;
        size_ = (rec.size < file.size() - rec.file_offset)
                    ? rec.size
                    : file.size() - rec.file_offset;
    }
    if (rec.fields_offset == 4 && size_ >= 4) {
        class_id_ = read_u32(rec_);
    }
}

const PfsField* RecordView::find(std::uint32_t tag) const {
    for (const PfsField& f : *fields_) {
        if (f.tag == tag) return &f;
    }
    return nullptr;
}

std::uint32_t RecordView::u32(std::uint32_t tag, std::uint32_t def) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset + 4 > size_) return def;
    return read_u32(rec_ + f->offset);
}

std::uint16_t RecordView::u16(std::uint32_t tag, std::uint16_t def) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset + 2 > size_) return def;
    return read_u16(rec_ + f->offset);
}

std::uint8_t RecordView::u8(std::uint32_t tag, std::uint8_t def) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset + 1 > size_) return def;
    return rec_[f->offset];
}

std::string RecordView::short_string(std::uint32_t tag) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset >= size_) return {};
    std::size_t p = f->offset;
    std::size_t avail = size_ - p;
    std::size_t len = rec_[p];
    ++p;
    --avail;
    if (len == 0xFF) {  // a long (u32-length) string follows
        if (avail < 4) return {};
        len = read_u32(rec_ + p);
        p += 4;
        avail -= 4;
    }
    if (len > avail) len = avail;
    return std::string(reinterpret_cast<const char*>(rec_ + p), len);
}

std::string RecordView::text(std::uint32_t tag) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset + 4 > size_) return {};
    std::size_t p = f->offset;
    std::size_t len = read_u32(rec_ + p);
    p += 4;
    if (len > size_ - p) len = size_ - p;
    return std::string(reinterpret_cast<const char*>(rec_ + p), len);
}

std::vector<std::uint8_t> RecordView::bytes(std::uint32_t tag) const {
    const PfsField* f = find(tag);
    if (f == nullptr || f->offset >= size_) return {};
    std::size_t n = f->size;
    if (f->offset + n > size_) n = size_ - f->offset;
    return std::vector<std::uint8_t>(rec_ + f->offset, rec_ + f->offset + n);
}

// ---------------------------------------------------------------------------
// Loaders
// ---------------------------------------------------------------------------
namespace {

// True when a record belongs to 'want'. Records without a class id pass
// (Ability.pfs / Spells.pfs take their element class from the list).
bool class_ok(const RecordView& v, std::uint32_t want) {
    return v.class_id() == 0 || v.class_id() == want;
}

}  // namespace

std::vector<Ability> load_abilities(const PfsFile& pfs) {
    std::vector<Ability> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    out.reserve(recs.size());
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        Ability a;
        a.index = i;
        a.tag = recs[i].tag;
        a.description = v.text(5);       // rwStrings (u32 length + bytes)
        a.cost = v.u32(6);               // rwInteger
        a.has_sound = v.boolean(7);      // rwEObject presence
        a.has_sequence = v.boolean(8);   // rwStreamProc presence
        a.move_types = v.u16(9);         // rwData (2 bytes)
        out.push_back(std::move(a));
    }
    return out;
}

std::vector<Spell> load_spells(const PfsFile& pfs) {
    std::vector<Spell> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    out.reserve(recs.size());
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        Spell s;
        s.index = i;
        s.tag = recs[i].tag;
        s.description = v.text(10);      // rwStrings
        s.effect_ref = v.bytes(11);      // rwEObject
        s.animation = v.bytes(12);       // rwStreamProc
        s.mana_cost = v.u32(13);         // rwInteger (+0x14)
        s.upkeep = v.u32(14);            // rwInteger (+0x1c)
        s.research_cost = v.u32(15);     // rwInteger (+0x18)
        s.sphere = v.u8(16);             // rwData(1) (+0x20)
        s.level = v.u8(17);              // rwByte (+0x21)
        out.push_back(std::move(s));
    }
    return out;
}

std::vector<UnitStat> load_units(const PfsFile& pfs) {
    std::vector<UnitStat> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kUnitResourceClassId)) continue;
        UnitStat u;
        u.index = i;
        u.tag = recs[i].tag;
        u.res_id = v.u32(1);
        u.res_type = v.u32(2);
        u.res_index = v.u32(3);
        u.gfx_index = v.u32(8);
        u.name = v.short_string(10);       // rwString (u8 length)
        u.qualifier = v.short_string(11);
        u.type = v.u8(12);
        u.race_id = v.u8(13);
        u.attack = v.u8(14);
        u.defense = v.u8(15);
        u.damage = v.u8(16);
        u.hit_points = v.u8(17);
        u.moves = v.u8(18);
        u.resistance = v.u8(19);
        u.level = v.u8(20);
        u.extra21 = v.u8(21);
        u.extra22 = v.u8(22);
        u.extra23 = v.u8(23);
        u.transport_capacity = v.u8(24);
        u.ability_ref = v.bytes(25);
        u.description = v.text(26);        // rwStrings (u32 length + bytes)
        u.obtain_value = v.u32(27);
        u.extra28 = v.u8(28);
        u.extra29 = v.u8(29);
        u.obj30 = v.bytes(30);
        u.obj31 = v.bytes(31);
        out.push_back(std::move(u));
    }
    return out;
}

std::vector<Item> load_items(const PfsFile& pfs) {
    std::vector<Item> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kItemClassId)) continue;
        Item it;
        it.index = i;
        it.tag = recs[i].tag;
        it.ability_count = v.u32(2);
        it.ability_mask = v.bytes(3);
        it.gfx_index = v.u32(7);
        it.name = v.short_string(8);
        it.description = v.short_string(9);
        it.flag10 = v.u8(10);
        it.flag11 = v.u8(11);
        it.flag12 = v.u8(12);
        it.flag13 = v.u8(13);
        it.flag14 = v.u8(14);
        it.item_type = v.u8(15);
        it.bonus = v.boolean(16);
        it.value17 = v.u32(17);
        it.text18 = v.text(18);
        it.obtain_value = v.u32(19);
        it.spell = v.u32(20);
        it.library_item = v.u32(21);
        it.library_name = v.short_string(22);
        out.push_back(std::move(it));
    }
    return out;
}

std::vector<Hero> load_heroes(const PfsFile& pfs) {
    std::vector<Hero> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kHeroClassId)) continue;
        Hero h;
        h.index = i;
        h.tag = recs[i].tag;
        h.ability_count = v.u32(2);
        h.ability_mask = v.bytes(3);
        h.value4 = v.u32(4);
        h.value5 = v.u32(5);
        h.morale = v.u8(6);
        h.face_index = v.u32(7);
        h.hero_resource_index = v.u32(8);
        h.value9 = v.u32(9);
        h.name = v.short_string(10);
        h.nickname = v.short_string(11);
        h.value12 = v.u8(12);
        h.casting_points = v.u32(13);
        h.value14 = v.u32(14);
        h.gender = v.u8(15);
        h.attack_skill = v.u8(16);
        h.defense_skill = v.u8(17);
        h.damage_skill = v.u8(18);
        h.hits_skill = v.u8(19);
        h.moves_skill = v.u8(20);
        h.resistance_skill = v.u8(21);
        h.unit_level = v.u8(22);
        h.value23 = v.boolean(23);
        h.experience = v.u32(24);
        h.value25 = v.u8(25);
        h.value26 = v.u32(26);
        h.value27 = v.u8(27);
        h.move_points = v.u8(29);
        h.hit_points = v.u8(30);
        h.value31 = v.u32(31);
        h.value33 = v.boolean(33);
        h.value34 = v.u32(34);
        h.library_hero = v.u32(35);
        h.ability_set_b = v.bytes(36);
        h.ability_set = v.bytes(37);
        h.library_name = v.short_string(38);
        h.used_skill_points = v.u32(39);
        out.push_back(std::move(h));
    }
    return out;
}

std::vector<HeroResource> load_hero_resources(const PfsFile& pfs) {
    std::vector<HeroResource> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kHeroResourceClassId)) continue;
        HeroResource h;
        h.index = i;
        h.tag = recs[i].tag;
        h.res_id = v.u32(1);
        h.res_type = v.u32(2);
        h.res_index = v.u32(3);
        h.gfx_index = v.u32(8);
        h.name = v.short_string(10);
        h.value11 = v.u8(11);
        h.value12 = v.u8(12);
        h.hero_class = v.u8(13);
        h.value14 = v.boolean(14);
        h.attack_skill = v.u8(15);
        h.defense_skill = v.u8(16);
        h.damage_skill = v.u8(17);
        h.hits_skill = v.u8(18);
        h.moves_skill = v.u8(19);
        h.resistance_skill = v.u8(20);
        h.obj29 = v.bytes(29);
        h.value30 = v.u32(30);
        out.push_back(std::move(h));
    }
    return out;
}

std::vector<UnitGfx> load_unit_gfx(const PfsFile& pfs) {
    std::vector<UnitGfx> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kUnitGfxClassId)) continue;
        UnitGfx g;
        g.index = i;
        g.tag = recs[i].tag;
        g.res_id = v.u32(1);
        g.res_type = v.u32(2);
        g.res_index = v.u32(3);
        g.sequence = v.bytes(5);
        g.image_lib = v.short_string(6);
        g.name = v.short_string(7);
        g.gfx_index = v.u32(8);
        g.value10 = v.u8(10);
        g.obj15 = v.bytes(15);
        g.obj16 = v.bytes(16);
        g.obj17 = v.bytes(17);
        g.obj18 = v.bytes(18);
        g.obj19 = v.bytes(19);
        out.push_back(std::move(g));
    }
    return out;
}

std::vector<FaceResource> load_faces(const PfsFile& pfs) {
    std::vector<FaceResource> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kFaceResourceClassId)) continue;
        FaceResource f;
        f.index = i;
        f.tag = recs[i].tag;
        f.res_id = v.u32(1);
        f.res_type = v.u32(2);
        f.res_index = v.u32(3);
        f.image_lib = v.short_string(5);
        f.face_index = v.u32(6);
        f.value7 = v.boolean(7);
        out.push_back(std::move(f));
    }
    return out;
}

std::vector<ItemGfx> load_item_gfx(const PfsFile& pfs) {
    std::vector<ItemGfx> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kItemGfxClassId)) continue;
        ItemGfx g;
        g.index = i;
        g.tag = recs[i].tag;
        g.res_id = v.u32(1);
        g.res_type = v.u32(2);
        g.res_index = v.u32(3);
        g.image_lib = v.short_string(6);
        g.gfx_index = v.u32(7);
        g.value8 = v.u8(8);
        g.value9 = v.boolean(9);
        out.push_back(std::move(g));
    }
    return out;
}

std::vector<TextItem> load_texts(const PfsFile& pfs) {
    std::vector<TextItem> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        if (!class_ok(v, kTextItemClassId)) continue;
        TextItem t;
        t.index = i;
        t.tag = recs[i].tag;
        t.id = v.u32(9);
        t.text = v.text(10);            // rwStrings (u32 + bytes)
        t.name = v.short_string(11);    // rwString (u8 len)
        out.push_back(std::move(t));
    }
    return out;
}

std::vector<TerrainRes> load_terrain(const PfsFile& pfs) {
    std::vector<TerrainRes> out;
    const std::vector<std::uint8_t>& file = pfs.bytes();
    const std::vector<PfsRecord>& recs = pfs.records();
    for (std::size_t i = 0; i < recs.size(); ++i) {
        RecordView v(recs[i], file);
        TerrainRes t;
        t.index = i;
        t.tag = recs[i].tag;
        t.res_id = v.u32(1);
        t.res_type = v.u32(2);
        t.res_index = v.u32(3);
        t.value20 = v.u32(0x14);
        out.push_back(std::move(t));
    }
    return out;
}

}  // namespace aow
