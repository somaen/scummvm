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

#ifndef TINSEL_TINSEL_H
#define TINSEL_TINSEL_H

#include "common/scummsys.h"
#include "common/system.h"
#include "common/error.h"
#include "common/events.h"
#include "common/keyboard.h"
#include "common/random.h"
#include "common/util.h"

#include "engines/engine.h"
#include "gui/debugger.h"

#include "tinsel/debugger.h"
#include "tinsel/graphics.h"
#include "tinsel/sound.h"
#include "tinsel/dw.h"
#include "tinsel/detection/detection_enums.h"

/**
 * This is the namespace of the Tinsel engine.
 *
 * Status of this engine: Complete
 *
 * Games using this engine:
 * - Discworld
 * - Discworld 2: Missing Presumed ...!?
 */
namespace Tinsel {

class BMVPlayer;
class Config;
class MidiDriver;
class MidiMusicPlayer;
class PCMMusicPlayer;
class Music;
class SoundManager;
class Background;
class Font;
class Cursor;

typedef Common::List<Common::Rect> RectList;

enum {
	kTinselDebugAnimations = 1 << 0,
	kTinselDebugActions = 1 << 1,
	kTinselDebugSound = 1 << 2,
	kTinselDebugMusic = 2 << 3
};

#define DEBUG_BASIC 1
#define DEBUG_INTERMEDIATE 2
#define DEBUG_DETAILED 3

struct TinselGameDescription;

enum TinselKeyDirection {
	MSK_LEFT = 1, MSK_RIGHT = 2, MSK_UP = 4, MSK_DOWN = 8,
	MSK_DIRECTION = MSK_LEFT | MSK_RIGHT | MSK_UP | MSK_DOWN
};

typedef bool (*KEYFPTR)(const Common::KeyState &);

#define	SCREEN_WIDTH	(_vm->screen().w)	// PC screen dimensions
#define	SCREEN_HEIGHT	(_vm->screen().h)
#define	SCRN_CENTER_X	((SCREEN_WIDTH  - 1) / 2)	// screen center x
#define	SCRN_CENTER_Y	((SCREEN_HEIGHT - 1) / 2)	// screen center y
#define UNUSED_LINES	48
#define EXTRA_UNUSED_LINES	3
//#define	SCREEN_BOX_HEIGHT1	(SCREEN_HEIGHT - UNUSED_LINES)
//#define	SCREEN_BOX_HEIGHT2	(SCREEN_BOX_HEIGHT1 - EXTRA_UNUSED_LINES)
#define	SCREEN_BOX_HEIGHT1	SCREEN_HEIGHT
#define	SCREEN_BOX_HEIGHT2	SCREEN_HEIGHT

#define GAME_FRAME_DELAY (1000 / ONE_SECOND)

#define TinselVersion (_vm->getVersion())
#define TinselV0 (TinselVersion == TINSEL_V0)
#define TinselV1 (TinselVersion == TINSEL_V1)
#define TinselV2 (TinselVersion == TINSEL_V2)
#define TinselV2Demo (TinselVersion == TINSEL_V2 && _vm->getIsADGFDemo())
#define TinselV1PSX (TinselVersion == TINSEL_V1 && _vm->getPlatform() == Common::kPlatformPSX)
#define TinselV1Mac (TinselVersion == TINSEL_V1 && _vm->getPlatform() == Common::kPlatformMacintosh)

#define READ_16(v) (TinselV1Mac ? READ_BE_UINT16(v) : READ_LE_UINT16(v))
#define READ_32(v) (TinselV1Mac ? READ_BE_UINT32(v) : READ_LE_UINT32(v))
#define FROM_16(v) (TinselV1Mac ? FROM_BE_16(v) : FROM_LE_16(v))
#define FROM_32(v) (TinselV1Mac ? FROM_BE_32(v) : FROM_LE_32(v))
#define TO_32(v)   (TinselV1Mac ? TO_BE_32(v) : TO_LE_32(v))

// Global reference to the TinselEngine object
extern TinselEngine *_vm;

class TinselEngine : public Engine {
	int _gameId;
	Common::KeyState _keyPressed;
	Common::RandomSource _random;
	Graphics::Surface _screenSurface;
	Common::Point _mousePos;
	uint8 _dosPlayerDir;

	static const char *const _sampleIndices[][3];
	static const char *const _sampleFiles[][3];
	static const char *const _textFiles[][3];

protected:

	// Engine APIs
	void initializePath(const Common::FSNode &gamePath) override;
	Common::Error run() override;
	bool hasFeature(EngineFeature f) const override;
	Common::Error loadGameState(int slot) override;
#if 0
	Common::Error saveGameState(int slot, const Common::String &desc, bool isAutosave = false);
#endif
	bool canLoadGameStateCurrently() override;
#if 0
	bool canSaveGameStateCurrently();
#endif

public:
	TinselEngine(OSystem *syst, const TinselGameDescription *gameDesc);
	~TinselEngine() override;
	int getGameId() {
		return _gameId;
	}

	const TinselGameDescription *_gameDescription;
	uint32 getGameID() const;
	uint32 getFeatures() const;
	Common::Language getLanguage() const;
	uint16 getVersion() const;
	Common::Platform getPlatform() const;
	bool getIsADGFDemo() const;
	bool isV1CD() const;

	const char *getSampleIndex(LANGUAGE lang);
	const char *getSampleFile(LANGUAGE lang);
	const char *getTextFile(LANGUAGE lang);

	MidiDriver *_driver;
	SoundManager *_sound;
	MidiMusicPlayer *_midiMusic;
	PCMMusicPlayer *_pcmMusic;
	Music *_music;
	BMVPlayer *_bmv;
	Background* _bg;
	Font *_font;
	Cursor *_cursor;

	Config *_config;

	KEYFPTR _keyHandler;

	/** Stack of pending mouse button events. */
	Common::List<Common::EventType> _mouseButtons;

	/** Stack of pending keypresses. */
	Common::List<Common::Event> _keypresses;


	/** List of all clip rectangles. */
	RectList _clipRects;

private:
	void NextGameCycle();
	void CreateConstProcesses();
	void RestartGame();
	void RestartDrivers();
	void ChopDrivers();
	void ProcessKeyEvent(const Common::Event &event);
	bool pollEvent();

public:
	const Common::String getTargetName() const { return _targetName; }
	Common::String getSavegameFilename(int16 saveNum) const;
	Common::SaveFileManager *getSaveFileMan() { return _saveFileMan; }
	Graphics::Surface &screen() { return _screenSurface; }

	Common::Point getMousePosition() const { return _mousePos; }
	void setMousePosition(Common::Point pt) {
		// Clip mouse position to be within the screen coordinates
		pt.x = CLIP<int16>(pt.x, 0, SCREEN_WIDTH - 1);
		pt.y = CLIP<int16>(pt.y, 0, SCREEN_HEIGHT - 1);

		int yOffset = TinselV2 ? (g_system->getHeight() - _screenSurface.h) / 2 : 0;
		g_system->warpMouse(pt.x, pt.y + yOffset);
		_mousePos = pt;
	}
	void divertKeyInput(KEYFPTR fptr) { _keyHandler = fptr; }
	int getRandomNumber(int maxNumber) { return _random.getRandomNumber(maxNumber); }
	uint8 getKeyDirection() const { return _dosPlayerDir; }
};

// Externally available methods
void CuttingScene(bool bCutting);
void CDChangeForRestore(int cdNumber);
void CdHasChanged();

} // End of namespace Tinsel

#endif /* TINSEL_TINSEL_H */
