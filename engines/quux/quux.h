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
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef QUUX_H
#define QUUX_H

#include "common/random.h"
#include "engines/engine.h"
#include "gui/debugger.h"

namespace Graphics {
	class Surface;
}

namespace Quux {

class Console;

// our engine debug channels
enum {
	kQuuxDebugExample = 1 << 0,
	kQuuxDebugExample2 = 1 << 1,
    kQuuxDebugInit = 1 << 2,
    kQuuxDebugGFX = 1 << 3
                         // next new channel must be 1 << 2 (4)
                         // the current limitation is 32 debug channels (1 << 31 is the last one)
};

class QuuxGFX;
class QuuxEngine : public Engine {
public:
	QuuxEngine(OSystem *syst);
	~QuuxEngine();

	virtual Common::Error run();

private:
	void init();
	void mainLoop();
	void leftClick(int x, int y);
	void rightClick(int x, int y);

	QuuxGFX *_gfx;
	Console *_console;

	// We need random numbers
	Common::RandomSource *_rnd;
};

// Example console class
class Console : public GUI::Debugger {
public:
	Console(QuuxEngine *vm) {}
	virtual ~Console(void) {}
};

} // End of namespace Quux

#endif

