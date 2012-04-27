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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "dcgf.h"
#include "BFader.h"
#include "BGame.h"
#include "PlatformSDL.h"
#include "common/util.h"

namespace WinterMute {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_PERSISTENT(CBFader, false)

//////////////////////////////////////////////////////////////////////////
CBFader::CBFader(CBGame *inGame): CBObject(inGame) {
	_active = false;
	_red = _green = _blue = 0;
	_currentAlpha = 0x00;
	_sourceAlpha = 0;
	_targetAlpha = 0;
	_duration = 1000;
	_startTime = 0;
	_system = false;
}


//////////////////////////////////////////////////////////////////////////
CBFader::~CBFader() {

}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Update() {
	if (!_active) return S_OK;

	int AlphaDelta = _targetAlpha - _sourceAlpha;

	uint32 time;

	if (_system) time = CBPlatform::GetTime() - _startTime;
	else time = Game->_timer - _startTime;

	if (time >= _duration) _currentAlpha = _targetAlpha;
	else {
		_currentAlpha = _sourceAlpha + (float)time / (float)_duration * AlphaDelta;
	}
	_currentAlpha = MIN((unsigned char)255, MAX(_currentAlpha, (byte )0)); // TODO: clean

	_ready = time >= _duration;
	if (_ready && _currentAlpha == 0x00) _active = false;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Display() {
	if (!_active) return S_OK;

	if (_currentAlpha > 0x00) return Game->_renderer->FadeToColor(DRGBA(_red, _green, _blue, _currentAlpha));
	else return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Deactivate() {
	_active = false;
	_ready = true;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::FadeIn(uint32 SourceColor, uint32 Duration, bool System) {
	_ready = false;
	_active = true;

	_red   = D3DCOLGetR(SourceColor);
	_green = D3DCOLGetG(SourceColor);
	_blue  = D3DCOLGetB(SourceColor);

	_sourceAlpha = D3DCOLGetA(SourceColor);
	_targetAlpha = 0;

	_duration = Duration;
	_system = System;

	if (_system) _startTime = CBPlatform::GetTime();
	else _startTime = Game->_timer;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::FadeOut(uint32 TargetColor, uint32 Duration, bool System) {
	_ready = false;
	_active = true;

	_red   = D3DCOLGetR(TargetColor);
	_green = D3DCOLGetG(TargetColor);
	_blue  = D3DCOLGetB(TargetColor);

	//_sourceAlpha = 0;
	_sourceAlpha = _currentAlpha;
	_targetAlpha = D3DCOLGetA(TargetColor);

	_duration = Duration;
	_system = System;

	if (_system) _startTime = CBPlatform::GetTime();
	else _startTime = Game->_timer;


	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
uint32 CBFader::GetCurrentColor() {
	return DRGBA(_red, _green, _blue, _currentAlpha);
}



//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Persist(CBPersistMgr *PersistMgr) {
	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(_active));
	PersistMgr->Transfer(TMEMBER(_blue));
	PersistMgr->Transfer(TMEMBER(_currentAlpha));
	PersistMgr->Transfer(TMEMBER(_duration));
	PersistMgr->Transfer(TMEMBER(_green));
	PersistMgr->Transfer(TMEMBER(_red));
	PersistMgr->Transfer(TMEMBER(_sourceAlpha));
	PersistMgr->Transfer(TMEMBER(_startTime));
	PersistMgr->Transfer(TMEMBER(_targetAlpha));
	PersistMgr->Transfer(TMEMBER(_system));

	if (_system && !PersistMgr->_saving) _startTime = 0;

	return S_OK;
}

} // end of namespace WinterMute
