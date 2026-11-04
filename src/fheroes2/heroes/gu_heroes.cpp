/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2026                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "gu_heroes.h"

#include <algorithm>

#include "heroes.h"
#include "monster.h"
#include "race.h"
#include "skill.h"
#include "translations.h"

namespace
{
    using Sec = Skill::Secondary;
    namespace Lvl = Skill::Level;

    // The eight Genuine Undead legendaries. Tweak the values here.
    // The race of each hero follows from the hero ID it replaces (see getHeroIdRangeForRace() and AllHeroes::Init() in heroes.cpp).
    const std::array<GU::HeroData, 8> guHeroes{ {
        // Faith: Wizard. Wisdom and spells.
        { Heroes::MYRA,
          Race::WZRD,
          gettext_noop( "Faith" ),
          gettext_noop( "Faith always believed in a happy ending, even after the end. Death only proved her right. She reads every spellbook twice: once for the "
                        "living and once for the undead." ),
          0,
          1,
          2,
          3,
          { { { Sec::WISDOM, Lvl::ADVANCED }, { Sec::MYSTICISM, Lvl::BASIC } } },
          { { { Monster::HALFLING, 15 }, { Monster::BOAR, 4 } } } },

        // Love: Sorceress. Diplomacy and Leadership.
        { Heroes::ASTRA,
          Race::SORC,
          gettext_noop( "Love" ),
          gettext_noop( "Monsters would rather join Love than fight her, and the dead are no exception. Rumour has it a legend was once traded away just to have "
                        "her." ),
          0,
          1,
          2,
          2,
          { { { Sec::DIPLOMACY, Lvl::ADVANCED }, { Sec::LEADERSHIP, Lvl::BASIC } } },
          { { { Monster::SPRITE, 20 }, { Monster::DWARF, 5 } } } },

        // Nature: Sorceress. Pathfinding and Scouting.
        { Heroes::NATASHA,
          Race::SORC,
          gettext_noop( "Nature" ),
          gettext_noop( "Nature walks through swamp and forest as if they were pavement. Roots step aside for her and the fog shows her the way. She was undead "
                        "long before it was fashionable." ),
          0,
          0,
          2,
          3,
          { { { Sec::PATHFINDING, Lvl::ADVANCED }, { Sec::SCOUTING, Lvl::BASIC } } },
          { { { Monster::SPRITE, 15 }, { Monster::DWARF, 3 } } } },

        // Power: Warlock. High spell power and Mysticism.
        { Heroes::ARIE,
          Race::WRLK,
          gettext_noop( "Power" ),
          gettext_noop( "Power has plenty of spell power and very little patience. Why talk your way through a battle when one spell will do?" ),
          0,
          0,
          4,
          2,
          { { { Sec::MYSTICISM, Lvl::ADVANCED }, { Sec::WISDOM, Lvl::BASIC } } },
          { { { Monster::CENTAUR, 8 }, { Monster::GARGOYLE, 3 } } } },

        // Strength: Barbarian. High attack.
        { Heroes::THUNDAX,
          Race::BARB,
          gettext_noop( "Strength" ),
          gettext_noop( "Strength solves problems with his fists. When that does not work, he uses both fists. Rise and shine... for we are dead, and now we are "
                        "undead." ),
          4,
          1,
          0,
          1,
          { { { Sec::LEADERSHIP, Lvl::BASIC }, { Sec::ARCHERY, Lvl::BASIC } } },
          { { { Monster::GOBLIN, 25 }, { Monster::ORC, 5 } } } },

        // Chaos: Warlock. Luck.
        { Heroes::ALAMAR,
          Race::WRLK,
          gettext_noop( "Chaos" ),
          gettext_noop( "Chaos wants every game to feel a little unpredictable. Sometimes the dice love her, sometimes they don't. She thinks that is the fun "
                        "part." ),
          1,
          0,
          3,
          2,
          { { { Sec::LUCK, Lvl::ADVANCED }, { Sec::SCOUTING, Lvl::BASIC } } },
          { { { Monster::CENTAUR, 8 }, { Monster::GARGOYLE, 3 } } } },

        // Life: Necromancer. Necromancy.
        { Heroes::ZOM,
          Race::NECR,
          gettext_noop( "Life" ),
          gettext_noop( "Life gives life back... to skeletons. To him every battlefield is a recruiting office. #UndeadArmy" ),
          1,
          0,
          2,
          2,
          { { { Sec::NECROMANCY, Lvl::ADVANCED }, { Sec::WISDOM, Lvl::BASIC } } },
          { { { Monster::SKELETON, 15 }, { Monster::ZOMBIE, 4 } } } },

        // War: Knight. The burned legendary. Ceallach is a Price of Loyalty hero, so War is never offered for hire
        // (see AllHeroes::GetHeroForHire() in heroes.cpp). He only appears when a map places him.
        { Heroes::CEALLACH,
          Race::KNGT,
          gettext_noop( "War" ),
          gettext_noop( "War was once traded away for Love, and then he vanished for good, sent to a place no one can reach. Some say he returns from the "
                        "abyss when the world forgets him. He cannot be hired. He can only be faced." ),
          3,
          3,
          1,
          1,
          { { { Sec::LEADERSHIP, Lvl::EXPERT }, { Sec::BALLISTICS, Lvl::ADVANCED } } },
          { { { Monster::SWORDSMAN, 20 }, { Monster::CAVALRY, 10 }, { Monster::PALADIN, 4 } } } },
    } };
}

namespace GU
{
    const HeroData * getHeroData( const int heroId )
    {
        const auto it = std::find_if( guHeroes.begin(), guHeroes.end(), [heroId]( const HeroData & data ) { return data.heroId == heroId; } );
        return it != guHeroes.end() ? &( *it ) : nullptr;
    }
}
