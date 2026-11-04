# Architectuur van fheroes2 (voor de GU-mod)

Dit document beschrijft hoe fheroes2 in elkaar zit, voor zover dat nodig is om Genuine Undead-helden toe te voegen.
Alle paden zijn relatief aan de root van de repo. Regelnummers gelden voor de stand van `gu-mod` op 4 november 2026
(fheroes2 1.1.17) en kunnen verschuiven; de functienamen zijn leidend.

## Snel antwoord: waar voeg ik een nieuwe held toe?

Een held raakt minstens deze plekken:

| Wat | Waar |
| --- | --- |
| Hero-ID (enum) | `src/fheroes2/heroes/heroes.h`, enum in `class Heroes`. Nieuwe helden **direct boven** `HEROES_COUNT` (zie commentaar "Put all new heroes just above this line"). |
| Standaardnaam | `defaultHeroNames` in `src/fheroes2/heroes/heroes.cpp` (array ter grootte `HEROES_COUNT`, dus vergeten = compileerfout). |
| Ras/klasse | Gewone helden: ID-bereik in `getHeroIdRangeForRace()` (`heroes.cpp`). Campagne- en PoL-helden: vast ras in `AllHeroes::Init()` (`heroes.cpp`). |
| Toevoegen aan de wereld | `AllHeroes::Init()` (`heroes.cpp`); vectorindex moet gelijk zijn aan het ID (`AllHeroes::Get`). |
| Groot portret | `ICN::getHeroPortraitIcnId()` in `src/fheroes2/agg/icn.cpp` (switch; zonder case volgt `assert` "Did you add a new hero?"). |
| Klein portret | `ICN::MINIPORT`, frame `heroId - 1`, in `Heroes::GetPortrait()` (`heroes.cpp`). |
| Huurbaar of niet | `AllHeroes::GetHeroForHire()` (`heroes.cpp`): alleen ID's tot en met `CELIA` zijn huurbaar. |
| Savegames | `operator<<`/`operator>>` voor `Heroes` en `AllHeroes` (`heroes.cpp`) en `src/fheroes2/system/save_format_version.h`. |

Details per onderwerp staan hieronder.

## 1. Mappenstructuur

| Map | Inhoud |
| --- | --- |
| `src/engine` | Platformlaag: SDL-scherm, audio, events, afbeeldingen en paletten, serialisatie, random, vertalingen, zlib. |
| `src/fheroes2` | Het spel zelf, opgedeeld in submappen (zie hieronder). Startpunt: `src/fheroes2/game/fheroes2.cpp`. |
| `src/tools` | Losse hulpprogramma's voor assets: `extractor`, `icn2img`, `h2dmgr`, `til2img`, `pal2img`, `xmi2midi`, `82m2wav`, `bin2txt`. |
| `src/thirdparty` | libsmacker (afspelen van SMK-video's). |
| `src/resources` | Icoontjes, Windows `.rc`, macOS `Info.plist`. |
| `files/` | Data die met het spel meegaat: `files/data/resurrection.h2d`, `files/lang/*.po`, soundfonts, timidity. |
| `maps/` | Twaalf eigen kaarten in het nieuwe `.fh2m`-formaat. **Let op:** hier komen ook de originele `.MP2`-kaarten terecht; die worden via `.gitignore` genegeerd. |
| `script/` | Bouw- en installatiescripts per platform, plus `script/homm2` (GOG-data kopiëren) en `script/tools` (code-format checks). |
| `docs/` | Ontwikkel- en installatiedocumentatie en de bronbestanden van de website. |
| `VisualStudio/` | Instellingen voor de Visual Studio-build; `VisualStudio/packages` (SDL2) wordt genegeerd. |

Submappen van `src/fheroes2`:

| Map | Inhoud |
| --- | --- |
| `agg` | Inlezen van de originele data (AGG-archief, ICN-ID's). |
| `ai` | Computerspeler op de kaart en in gevechten. |
| `army` | Legers en troepen. |
| `audio` | Muziek en geluid. |
| `battle` | Gevechtsengine. |
| `campaign` | Campagnes en scenario's. |
| `castle` | Kastelen en gebouwen. |
| `dialog` | Alle dialoogvensters. |
| `editor` | Kaarteditor. |
| `game` | Hoofdlus, menu's, sneltoetsen, `game_assets.cpp` (alle afbeeldingen), `game_static.cpp` (startwaarden per ras). |
| `gui` | Interface-onderdelen (knoppen, randen, cursor). |
| `h2d` | Laden van `resurrection.h2d` (extra afbeeldingen van fheroes2). |
| `heroes` | De klasse `Heroes`, heldendialoog, vaardigheden. |
| `kingdom` | Koninkrijken, kleuren, rekrutering in de taverne. |
| `maps` | Tegels, laden van `.MP2` en `.fh2m`, random map generator. |
| `monster` | Monsterdata. |
| `resource` | Grondstoffen en artefacten. |
| `spell` | Spreuken. |
| `system` | Instellingen, spelers, savegame-versie. |
| `world` | Het wereldobject, kaart laden (`world_loadmap.cpp`), padvinding. |

## 2. Waar helden gedefinieerd worden

### Hero-ID's
`src/fheroes2/heroes/heroes.h`, enum binnen `class Heroes`:

- `UNKNOWN` (0).
- 54 gewone helden, 9 per ras, in de volgorde Knight, Barbarian, Sorceress, Warlock, Wizard, Necromancer (`LORDKILBURN` t/m `CELIA`).
- 6 campagnehelden van Succession Wars (`ROLAND`, `CORLAGON`, `ELIZA`, `ARCHIBALD`, `HALTON`, `BRAX`).
- 11 helden uit Price of Loyalty (`SOLMYR` t/m `JARKONAS`).
- `DEBUG_HERO`.
- Commentaar "Put all new heroes just above this line", dan `HEROES_COUNT`.

### Naam
`defaultHeroNames` in `heroes.cpp` (array van `HEROES_COUNT` strings, met `gettext_noop` zodat ze vertaalbaar zijn).
Opgehaald via `Heroes::getDefaultName()`.

### Klasse (ras)
- Gewone helden: het ras volgt uit het ID-bereik, `getHeroIdRangeForRace()` in `heroes.cpp`.
- Campagne- en PoL-helden: vast in `AllHeroes::Init()` in `heroes.cpp`, bijvoorbeeld `Heroes( ROLAND, Race::WZRD, 5000 )`
  (het derde argument is startervaring).

### Startvaardigheden, startspreuk en startleger
Dit is **per ras, niet per held**:

- Primaire vaardigheden (Attack, Defense, Power, Knowledge): `Skill::Primary::LoadDefaults()` in `src/fheroes2/heroes/skill.cpp`.
- Secundaire vaardigheden: constructor `Skill::SecSkills::SecSkills( race )` in `skill.cpp`.
- Startspreuk: `GetInitialSpell()` in `skill.cpp`.
- Al deze waarden komen uit de tabel `factionProperties` in `src/fheroes2/game/game_static.cpp`
  (struct `Skill::FactionProperties` in `src/fheroes2/heroes/skill_static.h`).
- Startleger: `Army::Reset()` in `src/fheroes2/army/army.cpp` geeft troepen van niveau 1 van het eigen ras,
  en met 90% kans ook niveau 2.
- Startartefacten: geen, behalve voor `DEBUG_HERO` of als de kaart ze meegeeft.

Er is één voorbeeld van een held met eigen startwaarden: de `DEBUG_HERO`-tak in de constructor `Heroes::Heroes( heroId, race )`
in `heroes.cpp`. Dat is de logische plek om per held afwijkende waarden in te haken.

### Biografie
**Bestaat niet.** fheroes2 heeft geen biografieteksten en de heldendialoog (`src/fheroes2/heroes/heroes_dialog.cpp`)
heeft er geen ruimte voor. Voor een GU-biografie moet dus een nieuw tekstveld en een plek in de interface komen.

### Specialisaties
**Bestaan niet** (er is niets met "specialty" in de code). Mogelijke haakjes als we ze willen toevoegen:
de getters zoals `Heroes::GetAttack()`, `getMoraleWithModifiers()` en `getLuckWithModifiers()` in `heroes.cpp`
(die bouwen al uitlegtekst op voor modifiers) en `_levelUp()`. Elk nieuw veld op een held moet ook in de savegame.

### Kaartspecifieke helden
Een kaart kan een held aanpassen: `Heroes::LoadFromMP2()` (originele kaarten) en `Heroes::applyHeroMetadata()`
(`.fh2m`-kaarten), beide in `heroes.cpp`. `Maps::Map_Format::HeroMetadata` (`src/fheroes2/maps/map_format_info.h`)
kent onder meer `customName`, `customPortrait` (index van een bestaand portret, geen eigen afbeelding), leger, artefacten,
spreuken, vaardigheden en niveau.

## 3. Huren in de taverne

- `Kingdom::GetRecruits()` in `src/fheroes2/kingdom/kingdom.cpp` vult twee plekken:
  de eerste met een held van het eigen ras, de tweede met een willekeurige.
- Beide gaan via `AllHeroes::GetHeroForHire()` in `heroes.cpp`. Daar staat: alleen ID's tot en met `CELIA`
  ("Only regular (non-campaign) heroes are available for hire"). Verder moet de held vrij zijn (`isAvailableForHire()`).
- Elke week opnieuw: `Kingdoms::resetRecruits()` in `kingdom.cpp`.

**Gevolg voor de mod:** een held met een ID ná `CELIA` is vanzelf niet huurbaar en verschijnt alleen als een kaart of
campagne hem plaatst. Dat is precies wat we voor **War** willen. Voor de zeven huurbare GU-helden moet deze filter juist
worden aangepast, of moeten ze bestaande ID's overnemen.

## 4. Portretten en sprites

### Hoe afbeeldingen geladen worden
- `src/fheroes2/agg/agg.cpp`: zoekt `data/heroes2.agg` en optioneel `data/heroes2x.agg` (Price of Loyalty) en leest
  bestanden daaruit.
- ICN-ID's staan in `src/fheroes2/agg/icn.h`. Alles tot `LAST_VALID_FILE_ICN` komt uit het AGG-bestand;
  ID's daarna zijn "gegenereerd" door fheroes2 zelf.
- Alle afbeeldingen lopen via `Assets::getImage( icnId, index )` in `src/fheroes2/game/game_assets.cpp`.
  `loadICN()` leest uit de AGG en `processICN()` past daarna van alles aan: pixels repareren, paletten wisselen,
  nieuwe afbeeldingen opbouwen.
- Alles is 8-bit met het HoMM2-palet (`KB.PAL` uit de AGG, `fheroes2::setGamePalette()`). Elke afbeelding heeft
  een tweede laag voor transparantie en schaduw.

### Eigen afbeeldingen naast de originele data
Dat kan, maar niet "los neerzetten en klaar":

- **`files/data/resurrection.h2d`**: een eigen archief van fheroes2 met ongeveer 60 extra afbeeldingen
  (formaat in `src/engine/h2d_file.cpp`). Geladen in `src/fheroes2/h2d/h2d.cpp`.
  **De lijst met bestandsnamen staat vast in de code** (`resurrectionH2DFileListSample`); klopt die niet, dan start
  het spel niet. Nieuwe afbeeldingen toevoegen betekent dus ook die lijst aanpassen.
- **`h2dmgr`** (`src/tools/h2dmgr.cpp`): zet BMP/PNG om naar `.image`-items in een `.h2d`
  (`h2dmgr combine doel.h2d palet.pal invoer...`) en terug (`extract`).
- `processICN()` gebruikt die afbeeldingen via `fheroes2::h2d::readImage( naam, sprite )`.
- Het spel laadt zelf **nooit** PNG's tijdens het spelen. PNG-ondersteuning (`fheroes2::Load`) is er alleen voor
  de tools, en alleen met de build-optie `ENABLE_IMAGE`.

Conclusie: GU-portretten komen het netst in een **eigen `.h2d`-bestand of in `resurrection.h2d`**, met een kleine
aanpassing in `processICN()` of `Heroes::GetPortrait()` om ze te gebruiken.

## 5. Afmetingen

| Afbeelding | Bron | Grootte |
| --- | --- | --- |
| Groot portret | `ICN::PORT0000` t/m `PORT0070`, frame 0 | **101 × 93** px (gecontroleerd in `game_assets.cpp` en gebruikt in `heroes_dialog.cpp`) |
| Middelgroot portret | Op het moment zelf verkleind uit het grote portret (`Heroes::GetPortrait`, `PORT_MEDIUM`) | **50 × 47** px |
| Klein portret | `ICN::MINIPORT`, frame `heroId - 1` | **30 × 22** px |
| Held op de kaart | `KNGT32`, `BARB32`, `SORC32`, `WRLK32`, `WZRD32`, `NECR32` (en `BOAT32` op zee) | Geen vaste maat; frames hebben eigen breedte, hoogte en offset en worden in tegels van 32 px geknipt. 5 richtingen × 9 frames, links is gespiegeld. |
| Vlag bij held | `*_FLAG32` per kleur | Idem |
| Held in de editor | `ICN::MINIHERO` (6 kleuren × 7 rassen) | Ongeveer 50 px hoog |

Belangrijk: de kaart-sprite hoort bij het **ras**, niet bij de held. Alle Knights zien er op de kaart hetzelfde uit.
Een eigen kaart-sprite per GU-held zou dus nieuwe code vragen.

## 6. Teksten en vertalingen

- Teksten in de code staan in `_( "..." )` (vertaald) of `gettext_noop( "..." )` (alleen gemarkeerd).
  Macro's in `src/engine/translations.h`.
- Vertalingen: `files/lang/*.po` (24 talen). Bij het bouwen maakt `msgfmt` er `.mo`-bestanden van
  (zie `files/lang/Makefile`). Het spel laadt `files/lang/<taal>.mo` via `Settings::setGameLanguage()`.
- Er is geen Nederlandse vertaling. Engels is de brontaal in de code.
- Kaarten in `.fh2m` kunnen eigen teksten per taal bevatten (`MapFormat::translationInfo`).

## 7. Kaarteditor en random map generator

### Kaarteditor
- Code in `src/fheroes2/editor/`. Startpunt `Editor::menuMain()` in `editor_mainmenu.cpp`,
  hoofdklasse `Interface::EditorInterface` in `editor_interface.cpp`.
- Slaat op in het `.fh2m`-formaat: `src/fheroes2/maps/map_format_info.h/.cpp` (`loadMap`, `saveMap`).
  Het spel laadt zo'n kaart in `World::loadResurrectionMap()` (`src/fheroes2/world/world_loadmap.cpp`).
- Een held op de kaart zetten: `EditorInterface::_tryToPlaceObject()`. Klik je op een geplaatste held, dan opent
  de heldendialoog en kun je naam, portret, leger, artefacten, spreuken en vaardigheden aanpassen (`HeroMetadata`).
- Er is geen vlag "alleen vijand". Dat regel je via kleuren: een kleur die in de kaartinstellingen alleen voor de
  computer is (`computerPlayerColors`, `editor_map_specs_window.cpp`) levert een vijandige held op.
  Daarmee is een eindbaas-kaart voor **War** al grotendeels te bouwen zonder code.

### Random map generator
- Code in `src/fheroes2/maps/map_random_generator.cpp`, ingang `Maps::Random_Generator::generateMap()`.
  Aangeroepen vanuit de editor (`EditorInterface::generateRandomMap`).
- Werkwijze in het kort: regio's worden in ringen rond het midden gelegd en groeien stap voor stap tot de kaart vol is.
  Daarna komen terrein, kastelen voor de spelers, verbindingen, mijnen, schatten, decoratie en bewakende monsters.
  Alleen de indeling "mirrored" is af.

## 8. Wat er al is voor mods en eigen content

- **Geen modsysteem.** Er zijn geen JSON- of configbestanden voor helden, monsters of teksten; alles staat in C++.
- Wel bruikbaar:
  - `.fh2m`-kaarten uit de editor (eigen kaarten, zoals **GUniverse**, met eigen teksten en helden-aanpassingen).
  - `.h2d`-archieven met eigen afbeeldingen (met de vaste lijst als beperking) en de tool `h2dmgr`.
  - Zoekpaden voor data (`Settings::GetRootDirs()` in `src/fheroes2/system/settings.cpp`): de map van het programma,
    de omgevingsvariabele `FHEROES2_DATA`, en op Windows `%APPDATA%\fheroes2`.
- Alles wat verder gaat (nieuwe helden, biografie, art) vraagt dus een eigen build. Dat is voor ons geen probleem,
  want we bouwen toch zelf.

## 9. Waar het mis kan gaan

- **Savegames:** helden worden opgeslagen als lijst van `HEROES_COUNT` stuks (`operator<<`/`>>` voor `AllHeroes`).
  Extra helden betekent een nieuwe versie in `src/fheroes2/system/save_format_version.h` en een aanpassing bij laden,
  anders zijn oude saves onbruikbaar. Helden alleen **achteraan** toevoegen houdt bestaande ID's heel.
- **Campagnes** verwijzen naar helden op ID (`src/fheroes2/game/game_campaign.cpp`, `src/fheroes2/campaign/campaign_data.cpp`).
  Bestaande ID's niet verschuiven.
- **Harde grenzen** in de code: `CELIA` (huren), `JARKONAS` (portretkeuze in de editor en in `world_loadmap.cpp`),
  `getMaximumAllowedHeroes()` in `heroes.h` (rekent met `HEROES_COUNT - 2 - 17`).
- **Multiplayer:** fheroes2 heeft alleen hot-seat (spelers op één pc), dus versieverschillen tussen pc's spelen geen rol.

## 10. Bouwen en controleren

- Windows: `script\windows\install_packages.bat`, dan `fheroes2-vs2019.vcxproj` bouwen in Visual Studio,
  configuratie **Release-SDL2**, platform x64. Programma: `build\x64\Release-SDL2\fheroes2.exe`, starten met de
  repo-root als werkmap.
- Linux (cloud): `cmake -B build && cmake --build build` (nodig: SDL2-, SDL2_mixer-, zlib- en gettext-pakketten).
- Er zijn **geen unit tests**. CI controleert stijl met clang-format 17 (`script/tools/check_code_format.sh`,
  `.clang-format`), copyrightkoppen en bouwt op alle platforms.
