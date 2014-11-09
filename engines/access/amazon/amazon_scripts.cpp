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
#include "access/access.h"
#include "access/amazon/amazon_game.h"
#include "access/amazon/amazon_resources.h"
#include "access/amazon/amazon_scripts.h"

namespace Access {

namespace Amazon {

AmazonScripts::AmazonScripts(AccessEngine *vm) : Scripts(vm) {
	_game = (AmazonEngine *)_vm;

	_xTrack = 0;
	_yTrack = 0;
	_zTrack = 0;
	_xCam = 0;
	_yCam = 0;
	_zCam = 0;

	_pNumObj = 0;
	for (int i = 0; i < 32; i++) {
		_pImgNum[i] = 0;
		_pObject[i] = nullptr;
		_pObjX[i] = 0;
		_pObjY[i] = 0;
		_pObjZ[i] = 0;
	}

	for (int i = 0; i < 16; i++) {
		_pObjXl[i] = 0;
		_pObjYl[i] = 0;
	}
}

void AmazonScripts::cLoop() {
	searchForSequence();
	_vm->_images.clear();
	_vm->_buffer2.copyFrom(_vm->_buffer1);
	_vm->_oldRects.clear();
	_vm->_scripts->executeScript();
	_vm->plotList1();
	_vm->copyBlocks();
}

void AmazonScripts::mWhile1() {
	_vm->_screen->setDisplayScan();
	_vm->_screen->fadeOut();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 0);
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->_screen->forceFadeIn();
	
	Resource *spriteData = _vm->_files->loadFile(14, 6);
	_vm->_objectsTable[0] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2100;

	do {
		cLoop();
		_sequence = 2100;
	} while (_vm->_flags[52] == 1);

	_vm->_screen->copyFrom(_vm->_buffer1);
	_vm->_buffer1.copyFrom(_vm->_buffer2);

	_game->establish(-1, 14);

	spriteData = _vm->_files->loadFile(14, 7);
	_vm->_objectsTable[1] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_sound->playSound(0);
	_vm->_screen->setDisplayScan();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 1);
	_vm->_screen->setPalette();
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2200;

	_vm->_sound->queueSound(0, 14, 15);

	do {
		cLoop();
		_sequence = 2200;
	} while (_vm->_flags[52] == 2);

	_vm->_screen->setDisplayScan();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 2);
	_vm->_screen->setPalette();
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->freeCells();

	spriteData = _vm->_files->loadFile(14, 8);
	_vm->_objectsTable[2] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2300;
	_vm->_sound->playSound(0);

	do {
		cLoop();
		_sequence = 2300;
	} while (_vm->_flags[52] == 3);

	_vm->freeCells();
	spriteData = _vm->_files->loadFile(14, 9);
	_vm->_objectsTable[3] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_screen->setDisplayScan();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 3);
	_vm->_screen->setPalette();
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2400;

	do {
		cLoop();
		_sequence = 2400;
	} while (_vm->_flags[52] == 4);
}

void AmazonScripts::mWhile2() {
	_vm->_screen->setDisplayScan();
	_vm->_screen->fadeOut();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 0);
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->_screen->forceFadeIn();

	Resource *spriteData = _vm->_files->loadFile(14, 6);
	_vm->_objectsTable[0] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2100;

	do {
		cLoop();
		_sequence = 2100;
	} while (_vm->_flags[52] == 1);

	_vm->_screen->fadeOut();
	_vm->freeCells();
	spriteData = _vm->_files->loadFile(14, 9);
	_vm->_objectsTable[3] = new SpriteResource(_vm, spriteData);
	delete spriteData;

	_vm->_screen->setDisplayScan();
	_vm->_events->hideCursor();

	_vm->_files->loadScreen(14, 3);
	_vm->_screen->setPalette();
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	_vm->_events->showCursor();

	_vm->_screen->setIconPalette();
	_vm->_images.clear();
	_vm->_oldRects.clear();
	_sequence = 2400;

	do {
		cLoop();
		_sequence = 2400;
	} while (_vm->_flags[52] == 4);
}

void AmazonScripts::doFlyCell() {
	Plane &plane = _game->_plane;
	SpriteResource *sprites = _vm->_objectsTable[15];

	if (plane._pCount <= 40) {
		_vm->_buffer2.plotImage(sprites, 3, Common::Point(70, 74));
	} else if (plane._pCount <= 80) {
		_vm->_buffer2.plotImage(sprites, 6, Common::Point(70, 74));
	} else if (plane._pCount <= 120) {
		_vm->_buffer2.plotImage(sprites, 2, Common::Point(50, 76));
	} else if (plane._pCount <= 160) {
		_vm->_buffer2.plotImage(sprites, 14, Common::Point(63, 78));
	} else if (plane._pCount <= 200) {
		_vm->_buffer2.plotImage(sprites, 5, Common::Point(86, 74));
	} else if (plane._pCount <= 240) {
		_vm->_buffer2.plotImage(sprites, 0, Common::Point(103, 76));
	} else if (plane._pCount <= 280) {
		_vm->_buffer2.plotImage(sprites, 4, Common::Point(119, 77));
	} else {
		_vm->_buffer2.plotImage(sprites, 1, Common::Point(111, 77));
	}

	if (plane._planeCount == 11 || plane._planeCount == 12)
		++plane._position.y;
	else if (plane._planeCount >= 28)
		--plane._position.y;

	_vm->_buffer2.plotImage(sprites, 7, plane._position);
	_vm->_buffer2.plotImage(sprites, 8 + plane._propCount, Common::Point(
		plane._position.x + 99, plane._position.y + 10));
	_vm->_buffer2.plotImage(sprites, 11 + plane._propCount, Common::Point(
		plane._position.x + 104, plane._position.y + 18));

	if (++plane._planeCount >= 30)
		plane._planeCount = 0;
	if (++plane._propCount >= 3)
		plane._propCount = 0;

	++plane._xCount;
	if (plane._xCount == 1)
		++plane._position.x;
	else
		plane._xCount = 0;
}

void AmazonScripts::doFallCell() {
	if (_vm->_scaleI <= 0)
		return;

	_game->_destIn = &_game->_buffer2;

	// 115, 11, i
	_vm->_screen->plotImage(_vm->_objectsTable[20], _game->_plane._planeCount / 6, Common::Point(115, 11));
	_vm->_scaleI -= 3;
	_vm->_scale = _vm->_scaleI;
	_vm->_screen->setScaleTable(_vm->_scale);
	++_game->_plane._xCount;
	if (_game->_plane._xCount == 5)
		return;
	_game->_plane._xCount = 0;
	if (_game->_plane._planeCount == 18)
		_game->_plane._planeCount = 0;
	else
		_game->_plane._planeCount += 6;
}

void AmazonScripts::pan() {
	_zCam += _zTrack;
	_xCam += _xTrack;
	int tx = (_xCam << 8) / _zCam;
	_yCam += _yTrack;
	int ty = (_yCam << 8) / _zCam;

	if (_vm->_timers[24]._flag != 1) {
		++_vm->_timers[24]._flag;
		for (int i = 0; i < _pNumObj; i++) {
			_pObjZ[i] = _zTrack;
			_pObjXl[i] += tx * _zTrack;
			_pObjX[i] += _pObjXl[i];
			_pObjYl[i] += ty * _zTrack;
			_pObjY[i] += _pObjYl[i];
		}
	}

	for (int i = 0; i < _pNumObj; i++) {
		ImageEntry ie;
		ie._flags= 8;
		ie._position = Common::Point(_pObjX[i], _pObjY[i]);
		ie._offsetY = 0xFF;
		ie._spritesPtr = _pObject[i];
		ie._frameNumber = _pImgNum[i];
	}
}

void AmazonScripts::scrollFly() {
	_vm->copyBF1BF2();
	_vm->_newRects.clear();
	doFlyCell();
	_vm->copyRects();
	_vm->copyBF2Vid();
}

void AmazonScripts::scrollFall() {
	_vm->copyBF1BF2();
	_vm->_newRects.clear();
	doFallCell();
	_vm->copyRects();
	_vm->copyBF2Vid();
}

void AmazonScripts::scrollJWalk() {
	_vm->copyBF1BF2();
	_vm->_newRects.clear();
	_game->plotList();
	_vm->copyRects();
	_vm->copyBF2Vid();
}

void AmazonScripts::mWhileFly() {
	Screen &screen = *_vm->_screen;
	Player &player = *_vm->_player;
	EventsManager &events = *_vm->_events;
	Plane &plane = _game->_plane;

	events.hideCursor();
	screen.clearScreen();
	screen.setBufferScan();
	screen.fadeOut();
	screen._scrollX = 0;

	_vm->_room->buildScreen();
	_vm->copyBF2Vid();
	screen.fadeIn();
	_vm->_oldRects.clear();
	_vm->_newRects.clear();

	// KEYFLG = 0;

	screen._scrollRow = screen._scrollCol = 0;
	screen._scrollX = screen._scrollY = 0;
	player._rawPlayer = Common::Point(0, 0);
	player._scrollAmount = 1;

	plane._pCount = 0;
	plane._planeCount = 0;
	plane._propCount = 0;
	plane._xCount = 0;
	plane._position = Common::Point(20, 29);

	while (!_vm->shouldQuit() && !events.isKeyMousePressed() &&
			((screen._scrollCol + screen._vWindowWidth) != _vm->_room->_playFieldWidth)) {
		events._vbCount = 4;
		screen._scrollX += player._scrollAmount;

		while (screen._scrollX >= TILE_WIDTH) {
			screen._scrollX -= TILE_WIDTH;
			++screen._scrollCol;

			_vm->_buffer1.moveBufferLeft();
			_vm->_room->buildColumn(screen._scrollCol + screen._vWindowWidth, screen._vWindowBytesWide);
		}

		scrollFly();
		++plane._pCount;

		while (!_vm->shouldQuit() && events._vbCount > 0) {
			// To be rewritten when NEWTIMER is done
			events.checkForNextFrameCounter();
			_vm->_sound->playSound(0);

			g_system->delayMillis(10);
			events.pollEvents();
		}
	}

	events.showCursor();
}

void AmazonScripts::mWhileFall() {
	_vm->_events->hideCursor();
	_vm->_screen->clearScreen();
	_vm->_screen->setBufferScan();
	_vm->_screen->fadeOut();
	_vm->_screen->_scrollX = 0;

	_vm->_room->buildScreen();
	_vm->copyBF2Vid();
	_vm->_screen->fadeIn();
	_vm->_oldRects.clear();
	_vm->_newRects.clear();

	// KEYFLG = 0;

	_vm->_screen->_scrollRow = _vm->_screen->_scrollCol = 0;
	_vm->_screen->_scrollX = _vm->_screen->_scrollY = 0;
	_vm->_player->_scrollAmount = 3;
	_vm->_scaleI = 255;

	_game->_plane._xCount = 0;
	_game->_plane._planeCount = 0;

	while (true) {
		int _vbCount = 4;
		if (_vm->_screen->_scrollCol + _vm->_screen->_vWindowWidth == _vm->_room->_playFieldWidth) {
			_vm->_events->showCursor();
			return;
		}

		_vm->_screen->_scrollX += _vm->_player->_scrollAmount;
		while (_vm->_screen->_scrollX >= TILE_WIDTH) {
			_vm->_screen->_scrollX -= TILE_WIDTH;
			++_vm->_screen->_scrollCol;

			_vm->_buffer1.moveBufferLeft();
			_vm->_room->buildColumn(_vm->_screen->_scrollCol + _vm->_screen->_vWindowWidth, _vm->_screen->_vWindowBytesWide);
		}

		scrollFall();
		g_system->delayMillis(10);

		while(_vbCount > 0) {
			// To be rewritten when NEWTIMER is done
			_vm->_events->checkForNextFrameCounter();
			_vbCount--;
		}
	}
}

void AmazonScripts::mWhileJWalk() {
	const int jungleObj[7][4] = {
		{2, 77, 0, 40},
		{0, 290, 0, 50},
		{1, 210, 0, 70},
		{0, 50, 0, 30},
		{1, 70, 0, 20},
		{0, -280, 0, 60},
		{1, -150, 0, 30},
	};

	_vm->_screen->fadeOut();
	_vm->_events->hideCursor();
	_vm->_screen->clearScreen();
	_vm->_buffer2.clearBuffer();
	_vm->_screen->setBufferScan();
	_vm->_screen->_scrollX = 0;

	_vm->_room->buildScreen();
	_vm->copyBF2Vid();
	_vm->_screen->fadeIn();

	// KEYFLG = 0;
	_vm->_player->_xFlag = 1;
	_vm->_player->_yFlag = 0;
	_vm->_player->_moveTo.x = 160;
	_vm->_player->_move = UP;

	_game->_plane._xCount = 2;
	_xTrack = 10;
	_yTrack = _zTrack = 0;
	_xCam = 480;
	_yCam = 0;
	_zCam = 80;

	TimerEntry *te = &_vm->_timers[24];
	te->_initTm = te->_timer = 1;
	te->_flag++;
	
	_pNumObj = 7;
	for (int i = 0; i < _pNumObj; i++) {
		_pObject[i] = _vm->_objectsTable[24];
		_pImgNum[i] = jungleObj[i][0];
		_pObjX[i] = jungleObj[i][1];
		_pObjY[i] = jungleObj[i][2];
		_pObjZ[i] = jungleObj[i][3];
		_pObjXl[i] = _pObjYl[i] = 0;
	}
	
	while (true) {
		_vm->_images.clear();
		int _vbCount = 6;
		if (_vm->_player->_xFlag == 2) {
			_vm->_events->showCursor();
			return;
		}
		
		_pImgNum[0] = _game->_plane._xCount;
		if (_game->_plane._xCount == 2)
			++_game->_plane._xCount;
		else
			--_game->_plane._xCount;

		_vm->_player->checkMove();
		_vm->_player->checkScroll();
		pan();
		scrollJWalk();

		g_system->delayMillis(10);
		while(_vbCount > 0) {
			// To be rewritten when NEWTIMER is done
			_vm->_events->checkForNextFrameCounter();
			_vbCount--;
		}
	}
}

void AmazonScripts::mWhileDoOpen() {
	const int openObj[10][4] = {
		{8, -80, 120, 30},
		{13, 229, 0, 50},
		{12, 78, 0, 50},
		{11, 10, 0, 50},
		{10, 178, 97, 50},
		{9, 92, 192, 50},
		{14, 38, 0, 100},
		{15, 132, 76, 100},
		{16, 142, 0, 100},
		{4, -280, 40, 120},
	};

	_vm->_screen->setBufferScan();
	_vm->_events->hideCursor();
	_vm->_screen->forceFadeOut();
	_game->_skipStart = false;
	if (_vm->_conversation != 2) {
		_vm->_screen->setPanel(3);
		_game->startChapter(1);
		_game->establishCenter(0, 1);
	}

	Resource *data = _vm->_files->loadFile(0, 1);
	SpriteResource *spr = new SpriteResource(_vm, data);
	delete data;

	_vm->_objectsTable[1] = spr;
	_vm->_files->_setPaletteFlag = false;
	_vm->_files->loadScreen(1, 2);
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);

	warning("TODO _roomInfo = _vm->_files->loadFile(1, 1);");

	_xTrack = 8;
	_yTrack = -3;
	_zTrack = 0;
	_xCam = _yCam = 0;
	_zCam = 270;
	_vm->_timers[24]._timer = _vm->_timers[24]._initTm = 1;
	++_vm->_timers[24]._flag;
	_pNumObj = 10;

	for (int i = 0; i < _pNumObj; i++) {
		_pObject[i] = _vm->_objectsTable[1];
		_pImgNum[i] = openObj[i][0];
		_pObjX[i] = openObj[i][1];
		_pObjY[i] = openObj[i][2];
		_pObjZ[i] = openObj[i][3];
		_pObjXl[i] = _pObjYl[i] = 0;
	}

	_vm->_oldRects.clear();
	_vm->_newRects.clear();
	Animation *anim = _vm->_animation->setAnimation(0);
	_vm->_animation->setAnimTimer(anim);
	anim = _vm->_animation->setAnimation(1);
	_vm->_animation->setAnimTimer(anim);
	_vm->_sound->newMusic(10, 0);
	
	bool startFl = false;
	while (true) {
		_vm->_images.clear();
		_vm->_animation->animate(0);
		_vm->_animation->animate(1);
		pan();
		_vm->_buffer2.copyFrom(_vm->_buffer1);
		_vm->_newRects.clear();
		_game->plotList();
		_vm->copyBlocks();
		if (!startFl) {
			startFl = true;
			_vm->_screen->forceFadeIn();
		}
		_vm->_events->pollEvents();
		warning("TODO: check on KEYBUFCNT");
		if (_vm->_events->_leftButton || _vm->_events->_rightButton) {
			_game->_skipStart = true;
			_vm->_sound->newMusic(10, 1);
			break;
		}

		if (_xCam > 680) {
			warning("FIXME: _vbCount should be handled in NEWTIMER");
			int _vbCount = 125;
			while(_vbCount > 0) {
				// To be rewritten when NEWTIMER is done
				_vm->_events->checkForNextFrameCounter();
				_vbCount--;
			}
			break;
		}
	}
	
	_vm->_events->showCursor();
	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);
	warning("TODO: delete _roomInfo;");
	_vm->freeCells();
	_vm->_oldRects.clear();
	_vm->_newRects.clear();
	_vm->_numAnimTimers = 0;
	_vm->_images.clear();
	if (_vm->_conversation == 2) {
		Resource *spriteData = _vm->_files->loadFile(28, 37);
		_vm->_objectsTable[28] = new SpriteResource(_vm, spriteData);
		delete spriteData;
		warning("TODO: _roomInfo = _vm->_files->loadFile(28, 38);");
	}
}

void AmazonScripts::mWhile(int param1) {
	switch(param1) {
	case 1:
		mWhile1();
		break;
	case 2:
		mWhileFly();
		break;
	case 3:
		mWhileFall();
		break;
	case 4:
		mWhileJWalk();
		break;
	case 5:
		mWhileDoOpen();
		break;
	case 6:
		warning("TODO DOWNRIVER");
		break;
	case 7:
		mWhile2();
		break;
	case 8:
		warning("TODO JWALK2");
		break;
	default:
		break;
	}
}

void AmazonScripts::guardSee() {
	warning("TODO: guardSee()");
}

void AmazonScripts::setGuardFrame() {
	warning("TODO: setGuardFrame()");
}

void AmazonScripts::guard() {
	if (_vm->_timers[8]._flag != 0)
		return;

	++_vm->_timers[8]._flag;
	++_game->_guard._guardCel;
	int curCel = _game->_guard._guardCel;

	switch (_game->_guardLocation) {
	case 1:
		if (curCel <= 8 || curCel > 13)
			_game->_guard._guardCel = curCel = 8;

		_game->_guard._position.y = _vm->_player->_walkOffDown[curCel - 8];
		guardSee();
		if (_game->_guard._position.y >= 272) {
			_game->_guard._position.y = 272;
			_game->_guardLocation = 2;
		}
		break;
	case 2:
		if (curCel <= 43 || curCel > 48)
			_game->_guard._guardCel = curCel = 43;

		_game->_guard._position.x = _vm->_player->_walkOffLeft[curCel - 43];
		guardSee();
		if (_game->_guard._position.x <= 56) {
			_game->_guard._position.x = 56;
			_game->_guardLocation = 3;
		}
		break;
	case 3:
		if (curCel <= 0 || curCel > 5)
			_game->_guard._guardCel = curCel = 0;

		_game->_guard._position.y = _vm->_player->_walkOffUp[curCel];
		guardSee();
		if (_game->_guard._position.y <= 89) {
			_game->_guard._position.y = 89;
			_game->_guardLocation = 4;
			warning("CHECME: unused flag121");
		}
		break;
	default:
		if (curCel <= 43 || curCel > 48)
			_game->_guard._guardCel = curCel = 43;

		_game->_guard._position.x = _vm->_player->_walkOffRight[curCel - 43];
		guardSee();
		if (_game->_guard._position.x >= 127) {
			_game->_guard._position.x = 127;
			_game->_guardLocation = 1;
		}
		break;
	}

	setGuardFrame();
}

void AmazonScripts::loadBackground(int param1, int param2) {
	_vm->_files->_setPaletteFlag = false;
	_vm->_files->loadScreen(param1, param2);

	_vm->_buffer2.copyFrom(*_vm->_screen);
	_vm->_buffer1.copyFrom(*_vm->_screen);

	_vm->_screen->forceFadeIn();
}

void AmazonScripts::setInactive() {
	_game->_rawInactiveX = _vm->_player->_rawPlayer.x;
	_game->_rawInactiveY = _vm->_player->_rawPlayer.y;
	_game->_charSegSwitch = false;

	mWhile(_game->_rawInactiveY);
}

void AmazonScripts::boatWalls(int param1, int param2) {
	if (param1 == 1)
		_vm->_room->_plotter._walls[42] = Common::Rect(96, 27, 87, 42);
	else {
		_vm->_room->_plotter._walls[39].bottom = _vm->_room->_plotter._walls[41].bottom = 106;
		_vm->_room->_plotter._walls[40].left = 94;
	}
}

void AmazonScripts::plotInactive() {
	if (_game->_charSegSwitch) {
		_game->_currentCharFlag = true;
		SpriteResource *tmp = _vm->_inactive._spritesPtr;
		_vm->_inactive._spritesPtr = _vm->_player->_playerSprites;
		_vm->_player->_playerSprites = tmp;
		_game->_charSegSwitch = false;
	} else if (_game->_jasMayaFlag != (_game->_currentCharFlag ? 1 : 0)) {
		if (_vm->_player->_playerOff) {
			_game->_jasMayaFlag = (_game->_currentCharFlag ? 1 : 0);
		} else {
			_game->_currentCharFlag = (_game->_jasMayaFlag == 1);
			int tmpX = _game->_rawInactiveX;
			int tmpY = _game->_rawInactiveY;
			_game->_rawInactiveX = _vm->_player->_rawPlayer.x;
			_game->_rawInactiveY = _vm->_player->_rawPlayer.y;
			_vm->_player->_rawPlayer.x = tmpX;
			_vm->_player->_rawPlayer.y = tmpY;
			_game->_inactiveYOff = _vm->_player->_playerOffset.y;
			_vm->_player->calcManScale();
		}
	}

	if (_vm->_player->_roomNumber == 44) {
		warning("CHECKME: Only sets useless(?) flags 155 and 160");
	}

	_vm->_inactive._flags &= 0xFD;
	_vm->_inactive._flags &= 0xF7;
	_vm->_inactive._position.x = _game->_rawInactiveX;
	_vm->_inactive._position.y = _game->_rawInactiveY - _game->_inactiveYOff;
	_vm->_inactive._offsetY = _game->_inactiveYOff;
	_vm->_inactive._frameNumber = 0;

	_vm->_images.addToList(&_vm->_inactive);

}

void AmazonScripts::executeSpecial(int commandIndex, int param1, int param2) {
	switch (commandIndex) {
	case 1:
		_vm->establish(param1, param2);
		break;
	case 2:
		loadBackground(param1, param2);
		break;
	case 3:
		warning("TODO DOCAST");
		break;
	case 4:
		setInactive();
		break;
	case 6:
		mWhile(param1);
		break;
	case 9:
		guard();
		break;
	case 10:
		_vm->_sound->newMusic(param1, param2);
		break;
	case 11:
		plotInactive();
		break;
	case 13:
		warning("TODO RIVER");
		break;
	case 14:
		warning("TODO ANT");
		break;
	case 15:
		boatWalls(param1, param2);
		break;
	default:
		warning("Unexpected Special code %d - Skipped", commandIndex);
	}
}

typedef void(AmazonScripts::*AmazonScriptMethodPtr)();

void AmazonScripts::executeCommand(int commandIndex) {
	static const AmazonScriptMethodPtr COMMAND_LIST[] = {
		&AmazonScripts::cmdHelp, &AmazonScripts::cmdCycleBack,
		&AmazonScripts::cmdChapter, &AmazonScripts::cmdSetHelp,
		&AmazonScripts::cmdCenterPanel, &AmazonScripts::cmdMainPanel,
		&AmazonScripts::CMDRETFLASH
	};

	if (commandIndex >= 73)
		(this->*COMMAND_LIST[commandIndex - 73])();
	else
		Scripts::executeCommand(commandIndex);
}

void AmazonScripts::cmdHelp() {
	Common::String helpMessage = readString();

	if (_game->_helpLevel == 0) {
		_game->_timers.saveTimers();
		_game->_useItem = 0;

		if (_game->_noHints) {
			printString(NO_HELP_MESSAGE);
			return;
		} else if (_game->_hintLevel == 0) {
			printString(NO_HINTS_MESSAGE);
			return;
		}
	}

	int level = _game->_hintLevel - 1;
	if (level < _game->_helpLevel)
		_game->_moreHelp = 0;

	_game->drawHelp();
	error("TODO: more cmdHelp");
}

void AmazonScripts::cmdCycleBack() {
	if (_vm->_startup == -1)
		_vm->_screen->cyclePaletteBackwards();
}
void AmazonScripts::cmdChapter() {
	int chapter = _data->readByte();
	_game->startChapter(chapter);
}

void AmazonScripts::cmdSetHelp() {
	int arrayId = (_data->readUint16LE() && 0xFF) - 1;
	int helpId = _data->readUint16LE() && 0xFF;

	byte *help = _vm->_helpTbl[arrayId];
	help[helpId] = 1;

	if (_vm->_useItem == 0) {
		_sequence = 11000;
		searchForSequence();
	}
}

void AmazonScripts::cmdCenterPanel() {
	if (_vm->_screen->_vesaMode) {
		_vm->_screen->clearScreen();
		_vm->_screen->setPanel(3);
	}
}

void AmazonScripts::cmdMainPanel() {
	if (_vm->_screen->_vesaMode) {
		_vm->_room->init4Quads();
		_vm->_screen->setPanel(0);
	}
}

void AmazonScripts::CMDRETFLASH() { 
	error("TODO CMDRETFLASH"); 
}

} // End of namespace Amazon

} // End of namespace Access
