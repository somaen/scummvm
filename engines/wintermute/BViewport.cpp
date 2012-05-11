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

#include "BGame.h"
#include "PlatformSDL.h"
#include "BViewport.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CBViewport, false)

//////////////////////////////////////////////////////////////////////////
CBViewport::CBViewport(CBGame *inGame): CBBase(inGame) {
	CBPlatform::SetRectEmpty(&_rect);
	_mainObject = NULL;
	_offsetX = _offsetY = 0;
}


//////////////////////////////////////////////////////////////////////////
CBViewport::~CBViewport() {

}


//////////////////////////////////////////////////////////////////////////
HRESULT CBViewport::Persist(CBPersistMgr *PersistMgr) {

	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(_mainObject));
	PersistMgr->Transfer(TMEMBER(_offsetX));
	PersistMgr->Transfer(TMEMBER(_offsetY));
	PersistMgr->Transfer(TMEMBER(_rect));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBViewport::SetRect(int left, int top, int right, int bottom, bool NoCheck) {
	if (!NoCheck) {
		left = MAX(left, 0);
		top = MAX(top, 0);
		right = MIN(right, Game->_renderer->_width);
		bottom = MIN(bottom, Game->_renderer->_height);
	}

	CBPlatform::SetRect(&_rect, left, top, right, bottom);
	_offsetX = left;
	_offsetY = top;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
RECT *CBViewport::GetRect() {
	return &_rect;
}


//////////////////////////////////////////////////////////////////////////
int CBViewport::GetWidth() {
	return _rect.right - _rect.left;
}


//////////////////////////////////////////////////////////////////////////
int CBViewport::GetHeight() {
	return _rect.bottom - _rect.top;
}

} // end of namespace WinterMute
