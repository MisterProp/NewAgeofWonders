# 12 - The unit viewer (`aow_unitview`)

`aow_unitview.exe` is a small Win32 + GDI app that loads **every unit of the
game** and lets you browse them.

```
aow_unitview.exe [game folder]      (default: ..)
```

## Controls

| key | action |
|-----|--------|
| `<-` / `->` / mouse wheel | previous / next unit |
| `,` / `.` | step the sprite's frames (walk, attack, ...) |
| click, `Enter` | open the unit's page (and click again to go back) |
| `Esc` / `Backspace` | back to the browser / quit |
| `Home` / `End` | first / last unit |
| `+` / `-` | zoom the sprite |

The **browser** shows the unit's on-map **idle sprite** (its `UNITS\*.ILB`
frame 0) with the name and a quick stat block. Clicking (or `Enter`) opens the
**unit page**: the name, the full description, the eight combat stats
(attack / defense / damage / hits / moves / resistance / level / upkeep) and the
attributes (race, unit type, gender, size, blood type, alignment, transport,
obtain value, and the sprite / face libraries).

## Where the data comes from

The app is a thin UI over `aow::load_unit_catalog` (see `docs/11_UNIT_CATALOG.md`):

* **Unitres.pfs** - the unit records (name, race, stats, description),
* **Unitgfx.pfs** - the sprite reference; the on-map library (`UNITS\HMN\HMNAR.ILB`)
  is read out of the tag-5 animation sequence,
* **Release.hss** - the race (`RACES\R*.ILB`) list,
* **\*.ahl** - the leader/special lists.

Sprites are decoded with `aow::IlbFile::decode_rgb` (RGBA, alpha = transparent)
and alpha-composited over the panel background, because GDI's 32-bit `BI_RGB`
blit ignores the alpha channel. Each `UNITS\*.ILB` frame is 64x64.

## Head-less rendering

The same painting code can render a view to a PNG (no window), which is how the
build's smoke test exercises it:

```
aow_unitview.exe . --png out.png                 # the browser, unit 0
aow_unitview.exe . --png page.png --unit 18 --detail
aow_unitview.exe . --png walk.png  --unit 0 --frame 30
```

`--unit N` selects a unit by its position in `Unitres.pfs`, `--frame N` a sprite
frame of that unit's library. The write uses the port's stdlib-only PNG writer
(`aow::write_png_rgba`).

## Build

CMake target `aow_unitview` (Win32 subsystem; links `gdi32`, `user32`,
`shell32`), or `./build.ps1` which also builds + smoke-tests it. `ctest -R
unit_viewer` runs the two head-less render cases.
