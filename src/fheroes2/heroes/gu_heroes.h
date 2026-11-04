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

#pragma once

#include <array>
#include <cstdint>

// Genuine Undead (GU) mod: all data for the eight GU legendary heroes lives in gu_heroes.cpp.
// Each GU hero takes over an existing hero ID (option A in docs/gu-plan.md), so hero IDs,
// save files and campaigns are not affected.
namespace GU
{
    struct SecondarySkillEntry
    {
        int32_t skill{ 0 };
        int32_t level{ 0 };
    };

    struct TroopEntry
    {
        int32_t monster{ 0 };
        uint32_t count{ 0 };
    };

    struct HeroData
    {
        // Existing hero ID this GU hero replaces (Heroes::...).
        int heroId{ 0 };

        // Race (class) of the replaced hero (Race::...). GU values are only applied while the hero keeps this race.
        int race{ 0 };

        // Name and biography in English, marked for translation.
        const char * name{ nullptr };
        const char * biography{ nullptr };

        // Starting primary skills: attack, defense, spell power, knowledge.
        int32_t attack{ 0 };
        int32_t defense{ 0 };
        int32_t power{ 0 };
        int32_t knowledge{ 0 };

        // Starting secondary skills. Entries with skill 0 are ignored.
        std::array<SecondarySkillEntry, 4> secondarySkills{};

        // Starting army. Entries with monster 0 or count 0 are ignored.
        std::array<TroopEntry, 5> army{};
    };

    // Returns the GU data for the given hero ID, or nullptr if this hero is not a GU hero.
    const HeroData * getHeroData( const int heroId );
}
