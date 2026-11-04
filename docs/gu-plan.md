# GU-plan: hoe voegen we de Genuine Undead-helden toe?

Fase 2 van de GU-mod. Dit document vergelijkt twee manieren om de acht GU-legendaries als helden in fheroes2 te krijgen
en geeft een advies. Achtergrond over de code staat in [architectuur.md](architectuur.md).

## Uitgangspunten uit de code

- Elke held heeft een vast ID in de enum in `src/fheroes2/heroes/heroes.h`. Er zijn 54 gewone helden
  (9 per ras), 6 campagnehelden, 11 helden uit Price of Loyalty en een debug-held.
- Startvaardigheden, startspreuk en startleger gelden **per ras** (`factionProperties` in
  `src/fheroes2/game/game_static.cpp`, `Army::Reset()` in `src/fheroes2/army/army.cpp`). Per held bestaat er niets,
  behalve de speciale tak voor `DEBUG_HERO` in de constructor `Heroes::Heroes( heroId, race )` in `heroes.cpp`.
- Alleen helden met een ID tot en met `CELIA` zijn huurbaar (`AllHeroes::GetHeroForHire()` in `heroes.cpp`).
- Portretten komen uit de originele data: groot 101×93 (`ICN::PORTxxxx`), klein 30×22 (`ICN::MINIPORT`, frame `ID - 1`).
- De kaart-sprite hoort bij het ras, niet bij de held.
- Biografieën en specialisaties bestaan niet.
- Bij het begin van een nieuw spel kies je in HoMM2 een **ras**, geen held. Je startheld wordt willekeurig gekozen
  uit dat ras. "Een GU-held kiezen" betekent dus: hem huren in de taverne of het kasteel, of een kaart spelen waar hij
  al op staat. Wil je hem echt bij de start kiezen, dan is dat extra werk (zie "Open vragen").

## Optie A (simpel): bestaande helden ombouwen

Zeven gewone helden krijgen een GU-naam, GU-startwaarden en later een GU-portret. War neemt de plek in van een
niet-huurbare held.

Voorstel voor de koppeling (elk binnen het juiste ras):

| GU-held | Klasse | Vervangt | Waarom deze |
| --- | --- | --- | --- |
| Faith | Wizard | Myra (`MYRA`) | eerste Wizard |
| Love | Sorceress | Astra (`ASTRA`) | eerste Sorceress |
| Nature | Sorceress | Natasha (`NATASHA`) | tweede Sorceress |
| Power | Warlock | Arie (`ARIE`) | eerste Warlock |
| Strength | Barbarian | Thundax (`THUNDAX`) | eerste Barbarian |
| Chaos | Warlock | Alamar (`ALAMAR`) | tweede Warlock |
| Life | Necromancer | Zom (`ZOM`) | eerste Necromancer |
| War | Knight | Ceallach (`CEALLACH`) | Knight uit Price of Loyalty, niet huurbaar, niet gebruikt in campagnebeloningen |

**Wat moet er veranderen**

- Namen in `defaultHeroNames` (`heroes.cpp`).
- Eén nieuwe plek met GU-waarden per held (zie "Eén databestand" hieronder), toegepast in de constructor van `Heroes`.
- Portretten: in `processICN()` (`src/fheroes2/game/game_assets.cpp`) het portret van die ID's vervangen door GU-art
  uit een `.h2d`-bestand (fase 4).
- War: niets aan de huurlogica, hij is al niet huurbaar. Een eindbaas-kaart maken we in de editor met een kleur
  die alleen voor de computer is.

**Hoeveel werk:** klein. Ongeveer één werksessie voor namen en startwaarden, plus de art in fase 4.

**Wat kan stuk gaan**

- Savegames: niets. ID's en het aantal helden blijven gelijk, en namen worden als tekst opgeslagen.
- Campagnes: de campagnes verwijzen naar `ROLAND`, `ARCHIBALD`, `GALLAVANT`, `ELIZA`, `BRAX`, `CORLAGON`, `DAINWIN`,
  `JOSEPH` en `UNCLEIVAN` (`src/fheroes2/game/game_campaign.cpp`, `src/fheroes2/campaign/campaign_data.cpp`).
  Die laten we met rust. Wel kan een originele campagnekaart een van de omgebouwde helden plaatsen; die heet dan
  ineens "Faith". Dat is cosmetisch.
- Multiplayer: fheroes2 kent alleen hot-seat (alle spelers op één pc), dus geen versieproblemen tussen pc's.
- Je verliest zeven originele helden (en Ceallach) in je eigen build.

## Optie B (netjes): acht nieuwe helden erbij

Acht nieuwe ID's, toegevoegd direct boven `HEROES_COUNT` in `heroes.h`, zoals het commentaar daar voorschrijft.
Alle originele helden blijven bestaan.

**Wat moet er veranderen**

- Enum in `heroes.h`, namen in `defaultHeroNames`, aanmaken in `AllHeroes::Init()` met het juiste ras.
- Portretten: `ICN::getHeroPortraitIcnId()` in `src/fheroes2/agg/icn.cpp` heeft een case per held nodig, met nieuwe
  "gegenereerde" ICN's in `icn.h`. Het kleine portret (`MINIPORT`, frame `ID - 1`) heeft acht nieuwe frames nodig.
  Tot fase 4 hergebruiken we een bestaand portret.
- Huren: de filter in `AllHeroes::GetHeroForHire()` moet de zeven huurbare GU-helden toelaten (nu alleen tot `CELIA`).
  War blijft erbuiten.
- Harde grenzen bijwerken: `AllHeroes::getMaximumAllowedHeroes()` (`heroes.h`), de portretlijst van de editor
  in `Dialog::selectHeroes` (`src/fheroes2/dialog/dialog_selectitems.cpp`, nu tot `JARKONAS`) en `isPoLPortrait()`.
- Savegames: oude saves bevatten minder helden dan `HEROES_COUNT`. Dat vraagt een nieuwe versie in
  `src/fheroes2/system/save_format_version.h` en aanvullen bij het laden in `operator>>( IStreamBase &, AllHeroes & )`
  in `heroes.cpp`. Er is een voorbeeld van zo'n omzetting voor `FORMAT_VERSION_1010_RELEASE`.

**Hoeveel werk:** middel. Ongeveer twee à drie keer optie A, omdat er meer plekken in de code geraakt worden
en elke plek een kans op een `assert` of crash geeft.

**Wat kan stuk gaan**

- Savegames: zonder de omzetting crasht het laden van een oude save. Met omzetting gaat het goed.
  Saves van de GU-mod zijn daarna niet meer te openen in de gewone fheroes2.
- Campagnes: niets, de bestaande ID's veranderen niet.
- Kaarten: originele `.MP2`-kaarten kennen de nieuwe helden niet. Ze verschijnen alleen via de taverne of via
  eigen `.fh2m`-kaarten.
- Upstream bijhouden: meer conflicten bij het binnenhalen van nieuwe fheroes2-versies, omdat we op meer plekken in
  hun code zitten.

## Specialisaties

HoMM2 en fheroes2 hebben geen specialisaties. Toevoegen is mogelijk, maar het is een flinke klus: een nieuw veld per
held, opslaan in de savegame, het effect inbouwen in de getters (`Heroes::GetAttack()`, `getLuckWithModifiers()` enz.
in `heroes.cpp`) en het tonen in de heldendialoog.

**Advies: niet doen.** Het thema past juist heel goed bij wat HoMM2 al heeft. Bijna alle gewenste eigenschappen
bestaan als secundaire vaardigheid (`Skill::Secondary` in `src/fheroes2/heroes/skill.h`): Pathfinding, Scouting,
Diplomacy, Leadership, Wisdom, Mysticism, Luck en Necromancy. Het thema loopt dan via klasse, primaire vaardigheden,
startvaardigheden en startleger.

Concreet voorstel (fase 3 verfijnt de getallen):

| Held | Klasse | Startvaardigheden | Accent |
| --- | --- | --- | --- |
| Faith | Wizard | Wisdom (gevorderd), Mysticism | extra Knowledge |
| Love | Sorceress | Diplomacy (gevorderd), Leadership | groter startleger |
| Nature | Sorceress | Pathfinding (gevorderd), Scouting | |
| Power | Warlock | Mysticism (gevorderd), Wisdom | extra Spell Power |
| Strength | Barbarian | Leadership, Archery | extra Attack |
| Chaos | Warlock | Luck (gevorderd), Scouting | |
| Life | Necromancer | Necromancy (gevorderd), Wisdom | skeletten in het startleger |
| War | Knight | Leadership (expert), Ballistics | hoog niveau, sterk leger (eindbaas) |

Kanttekening: "Offense" bestaat niet in HoMM2 (dat is HoMM3). Voor Strength gebruiken we daarom een hoge Attack
plus Leadership en Archery.

## Biografie

Er is geen veld en geen plek in de interface voor een biografie. Toevoegen kost een tekst per held in het databestand
en een kleine uitbreiding van de heldendialoog (`src/fheroes2/heroes/heroes_dialog.cpp`), bijvoorbeeld een venster
bij een klik op het portret. Dat geldt voor beide opties even zwaar en raakt de savegame niet.

## Eén databestand

Voor beide opties zetten we alle GU-waarden in één nieuw bestand, bijvoorbeeld `src/fheroes2/heroes/gu_heroes.cpp`
met een tabel per held: naam, ras, primaire vaardigheden, secundaire vaardigheden, startleger, huurbaar ja/nee,
biografie. De rest van de code leest uit die tabel. Zo kun je later makkelijk schuiven met waardes, en is overstappen
van A naar B vooral een kwestie van de koppeling naar ID's aanpassen.

## Advies

**Begin met optie A**, met het databestand vanaf de start.

- Het is het kleinste stapje naar "ik kan een GU-held huren", met bijna geen risico voor savegames en campagnes.
- War komt gratis mee als niet-huurbare held.
- Wat we bouwen (databestand, startwaarden, biografie, portretten) is bij optie B precies hetzelfde.
  Willen we later de originele helden terug, dan stappen we over op B zonder iets weg te gooien.

Kies **optie B** als je de 54 originele helden in elk geval wilt houden. Dat kost meer werk en raakt de savegames.

## Open vragen voor Tim

1. Optie A of B?
2. Bij optie A: akkoord met de koppeling in de tabel (welke originele held wordt welke GU-held)?
3. Wil je de GU-helden ook bij de start van een nieuw spel kunnen kiezen? HoMM2 laat alleen een ras kiezen.
   Een eenvoudige tussenweg: in de taverne gaan GU-helden voor op gewone helden van hetzelfde ras.

## Later (niet nu bouwen)

- Een eigen GU-factie of stad. Veel art-werk: kasteelscherm, gebouwen, kaart-sprites, monsters. Pas als de rest staat.
- Een korte campagne rond de terugkeer van War. Kan als serie `.fh2m`-kaarten; de campagnestructuur zelf staat in
  `src/fheroes2/campaign/`.
- Classic en cyberpunk als twee varianten van de Necromancer-stad, bijvoorbeeld via paletwissels zoals fheroes2 die
  al gebruikt in `processICN()`.
- "Cyber"-skeletten als neutraal monster (fase 5, alleen als het simpel kan).
