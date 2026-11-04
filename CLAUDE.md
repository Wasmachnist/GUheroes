# CLAUDE.md: GU-mod van fheroes2

Dit is een fork van fheroes2 (open-source herbouw van Heroes of Might and Magic II) met een Genuine Undead (GU)-thema:
de acht GU-legendaries worden helden. War is "verbrand": niet huurbaar, alleen vijand of eindbaas.

## Werkafspraken

- Werk op branch `gu-mod` (fork: `Wasmachnist/GUheroes`, upstream: `ihhub/fheroes2`). Kleine commits.
- Documentatie in het Nederlands, code en commentaar in het Engels.
- Werk in fases (zie de taak-documentatie) en stop na elke fase voor review.
- Geen wijzigingen aan de spellogica vóór fase 3.
- Nooit originele HoMM2-data committen (`anim/`, `data/`, `music/`, `*.agg`, `*.mp2`, ...) of `gu-art/source/`.
- Gok niet hoe fheroes2 werkt: zoek het op en noem bestand en functie.

## Bouwen

- **Windows (Tims pc):** `script\windows\install_packages.bat` (eenmalig, SDL2 in `VisualStudio\packages`),
  dan `fheroes2-vs2019.vcxproj` bouwen met configuratie **Release-SDL2** en platform **x64** (toolset v142).
  Uitvoer: `build\x64\Release-SDL2\fheroes2.exe`. Starten met de repo-root als werkmap, anders vindt hij de data niet.
- **Linux (cloud):** pakketten `libsdl2-dev libsdl2-mixer-dev zlib1g-dev gettext`, dan `cmake -B build && cmake --build build`.
- **Game data:** `script\homm2\extract_homm2_resources.bat` vanuit de source tree kopieert `anim`, `data`, `maps`, `music`
  uit de GOG-installatie naar de repo-root.

## Testen

- Er zijn geen unit tests. Testen = bouwen, spel starten, nieuw spel beginnen en een kaart spelen.
- Stijl: clang-format 17 met `.clang-format` (`script/tools/check_code_format.sh`), let op `foo( a, b )`-spaties.
- Nieuwe bronbestanden hebben de copyrightkop nodig (`script/tools/copyright_headers`).

## Waar zit wat

Uitgebreid in [docs/architectuur.md](docs/architectuur.md). De belangrijkste plekken:

- Hero-ID's: enum in `src/fheroes2/heroes/heroes.h` (nieuwe helden direct boven `HEROES_COUNT`).
- Namen: `defaultHeroNames` in `src/fheroes2/heroes/heroes.cpp`.
- Ras per held: `getHeroIdRangeForRace()` en `AllHeroes::Init()` in `heroes.cpp`.
- Startwaarden per ras (niet per held): `factionProperties` in `src/fheroes2/game/game_static.cpp`; leger: `Army::Reset()`.
- Huren: `AllHeroes::GetHeroForHire()` in `heroes.cpp` (alleen ID's t/m `CELIA`).
- Portretten: `ICN::getHeroPortraitIcnId()` in `src/fheroes2/agg/icn.cpp`, `Heroes::GetPortrait()` in `heroes.cpp`.
  Groot 101×93, middel 50×47 (verkleind), klein 30×22 (`ICN::MINIPORT`).
- Afbeeldingen: `src/fheroes2/game/game_assets.cpp` (`loadICN`, `processICN`); extra art via `.h2d` (`src/fheroes2/h2d/h2d.cpp`, tool `h2dmgr`).
- Savegame-versie: `src/fheroes2/system/save_format_version.h`.
- Kaarteditor: `src/fheroes2/editor/`; kaartformaat `.fh2m`: `src/fheroes2/maps/map_format_info.h`.
- Teksten: `_()` / `gettext_noop`, vertalingen in `files/lang/*.po`.
