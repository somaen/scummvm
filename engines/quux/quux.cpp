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


#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/EventRecorder.h"
#include "common/file.h"
#include "common/fs.h"

#include "engines/util.h"

#include "quux/quux.h"
#include "quux/gfx.h"
#include "quux/sound.h"

namespace Quux {

QuuxEngine::QuuxEngine(OSystem *syst)
	: Engine(syst) {
	// Put your engine in a sane state, but do nothing big yet;
	// in particular, do not load data from files; rather, if you
	// need to do such things, do them from run().

	// Do not initialize graphics here

	// However this is the place to specify all default directories
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "sound");

	// Here is the right place to set up the engine specific debug channels
	DebugMan.addDebugChannel(kQuuxDebugExample, "example", "this is just an example for a engine specific debug channel");
	DebugMan.addDebugChannel(kQuuxDebugExample2, "example2", "also an example");

	// Don't forget to register your random source
	_rnd = new Common::RandomSource("quux");
	_gfx = NULL;
	_sound = NULL;

	debug("QuuxEngine::QuuxEngine");
}

QuuxEngine::~QuuxEngine() {
	debug("QuuxEngine::~QuuxEngine");

	// Dispose your resources here
	delete _rnd;
	delete _gfx;
	delete _sound;

	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();
}

Common::Error QuuxEngine::run() {
	// Initialize graphics using following:
	initGraphics(640, 480, false);

	// You could use backend transactions directly as an alternative,
	// but it isn't recommended, until you want to handle the error values
	// from OSystem::endGFXTransaction yourself.
	// This is just an example template:
	//_system->beginGFXTransaction();
	//  // This setup the graphics mode according to users seetings
	//  initCommonGFX(false);
	//
	//  // Specify dimensions of game graphics window.
	//  // In this example: 320x200
	//  _system->initSize(320, 200);
	//FIXME: You really want to handle
	//OSystem::kTransactionSizeChangeFailed here
	//_system->endGFXTransaction();

	// Create debugger console. It requires GFX to be initialized
	_console = new Console(this);

	// Additional setup.
	debug("QuuxEngine::init");

	// Your main even loop should be (invoked from) here.
	debug("QuuxEngine::go: Hello, World!");

	// This test will show up if -d1 and --debugflags=example are specified on the commandline
	debugC(1, kQuuxDebugExample, "Example debug call");

	// This test will show up if --debugflags=example or --debugflags=example2 or both of them and -d3 are specified on the commandline
	debugC(3, kQuuxDebugExample | kQuuxDebugExample2, "Example debug call two");

	warning("These calls are always printed"); // and you can use format-strings in them too:
	warning("%s %d %d", "Example:", 320, 240); // there is also error() which quits ScummVM after printing.

	init();
	mainLoop();

	return Common::kNoError;
}

void QuuxEngine::init() {
	_gfx = new QuuxGFX();
	_gfx->init();

	_sound = new QuuxSound();
}

void QuuxEngine::leftClick(int x, int y) {
	_gfx->drawImageAt(x, y);
}

void QuuxEngine::rightClick(int x, int y) {
	_gfx->clearScreen();
}

void QuuxEngine::mainLoop() {
	_sound->playIfAvailable("example.mp3");

	while (true) {
		// Handle any events
		Common::Event event;
		// Work through the event-queue.
		while (_eventMan->pollEvent(event)) {
			_eventMan->pollEvent(event);
			switch (event.type) {
				// Events are similar to SDL's events, see common/events.h
			case Common::EVENT_LBUTTONDOWN:
				leftClick(event.mouse.x, event.mouse.y);
				break;
			case Common::EVENT_RBUTTONDOWN:
				rightClick(event.mouse.x, event.mouse.y);
			case Common::EVENT_QUIT:
			case Common::EVENT_RTL:
				// Return-To-Launcher-specifics happen here, if you
				// support RTL.
				return;
			default:
				break;
			}
		}
		// It's a good idea to cap your framerate, additional logic for finding your current FPS
		// can be done here, for now, let's just make sure we rest a milli-second per loop
		// Otherwise we might have used getMillis() to compare to the last iteration for instance.
		_system->delayMillis(1);
	}
}

} // End of namespace Quux

