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

#ifndef WINTERMUTE_UIEDIT_H
#define WINTERMUTE_UIEDIT_H

#include "persistent.h"
#include "UIObject.h"
#include "common/events.h"

namespace WinterMute {
class CBFont;
class CUIEdit : public CUIObject {
public:
	DECLARE_PERSISTENT(CUIEdit, CUIObject)
	int _maxLength;
	int InsertChars(int Pos, byte *Chars, int Num);
	int DeleteChars(int Start, int End);
	bool _cursorVisible;
	uint32 _lastBlinkTime;
	virtual HRESULT Display(int OffsetX, int OffsetY);
	virtual bool HandleKeypress(Common::Event *event);
	int _scrollOffset;
	int _frameWidth;
	uint32 _cursorBlinkRate;
	void SetCursorChar(char *Char);
	char *_cursorChar;
	int _selEnd;
	int _selStart;
	CBFont *_fontSelected;
	CUIEdit(CBGame *inGame);
	virtual ~CUIEdit();

	HRESULT LoadFile(char *Filename);
	HRESULT LoadBuffer(byte  *Buffer, bool Complete = true);
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);

	// scripting interface
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char *ScToString();
};

} // end of namespace WinterMute

#endif
