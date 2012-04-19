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

#ifndef WINTERMUTE_BEVENT_H
#define WINTERMUTE_BEVENT_H


#include "BBase.h"
#include "dctypes.h"    // Added by ClassView

namespace WinterMute {

class CBEvent : public CBBase {
public:
	DECLARE_PERSISTENT(CBEvent, CBBase)
	void SetScript(char *Script);
	void SetName(char *Name);
	static const char *GetEventName(TEventType Type);
	char *m_Script;
	char *m_Name;
	TEventType m_Type;
	CBEvent(CBGame *inGame);
	CBEvent(CBGame *inGame, TEventType Type, char *Script);
	virtual ~CBEvent();
	HRESULT LoadFile(char *Filename);
	HRESULT LoadBuffer(byte  *Buffer, bool Complete = true);
};

} // end of namespace WinterMute

#endif
