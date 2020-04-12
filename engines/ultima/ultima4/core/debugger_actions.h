/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ULTIMA4_CORE_DEBUGGER_ACTIONS_H
#define ULTIMA4_CORE_DEBUGGER_ACTIONS_H

#include "ultima/ultima4/core/coords.h"
#include "ultima/ultima4/core/types.h"
#include "ultima/shared/engine/debugger.h"

namespace Ultima {
namespace Ultima4 {

/**
 * This is a secondary class inherited by the Debugger class
 * that contains various support methods for implementing the
 * different actions players can take in the game
 */
class DebuggerActions {
protected:
	/**
	 * Returns true if the debugger is active
	 */
	virtual bool isDebuggerActive() const = 0;

	/**
	 * Prints a message to the console if it's active, or to the
	 * game screen if not
	 */
	virtual void print(const char *fmt, ...) = 0;

	/**
	 * Prints a message to the console if it's active, or to the
	 * game screen if not
	 */
	virtual void printN(const char *fmt, ...) = 0;
public:
	/**
	 * Summons a creature given by 'creatureName'. This can either be given
	 * as the creature's name, or the creature's id.  Once it finds the
	 * creature to be summoned, it calls gameSpawnCreature() to spawn it.
	 */
	void summonCreature(const Common::String &name);

	/**
	 * Destroy object at a given co-ordinate
	 */
	bool destroyAt(const Coords &coords);

	/**
	 * Returns a direction from a given string
	 */
	Direction directionFromName(const Common::String &dirStr);

	/**
	 * Attempts to attack a creature at map coordinates x,y.  If no
	 * creature is present at that point, zero is returned.
	 */
	bool attackAt(const Coords &coords);
};

} // End of namespace Ultima4
} // End of namespace Ultima

#endif