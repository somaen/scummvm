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

#ifndef WINTERMUTE_BREGION_H
#define WINTERMUTE_BREGION_H

#include "engines/wintermute/BPoint.h"
#include "engines/wintermute/BObject.h"

namespace WinterMute {

class CBRegion : public CBObject {
public:
	float m_LastMimicScale;
	int m_LastMimicX;
	int m_LastMimicY;
	void Cleanup();
	HRESULT Mimic(CBRegion *Region, float Scale = 100.0f, int X = 0, int Y = 0);
	HRESULT GetBoundingRect(RECT *Rect);
	bool PtInPolygon(int X, int Y);
	DECLARE_PERSISTENT(CBRegion, CBObject)
	bool m_Active;
	int m_EditorSelectedPoint;
	CBRegion(CBGame *inGame);
	virtual ~CBRegion();
	bool PointInRegion(int X, int Y);
	bool CreateRegion();
	HRESULT LoadFile(char *Filename);
	HRESULT LoadBuffer(byte  *Buffer, bool Complete = true);
	RECT m_Rect;
	CBArray<CBPoint *, CBPoint *> m_Points;
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent, char *NameOverride = NULL);

	// scripting interface
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char *ScToString();
};

} // end of namespace WinterMute

#endif
