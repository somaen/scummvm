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

#ifndef __WmeAdWaypointGroup_H__
#define __WmeAdWaypointGroup_H__

#include "BObject.h"

namespace WinterMute {
class CBPoint;
class CAdWaypointGroup : public CBObject {
public:
	float m_LastMimicScale;
	int m_LastMimicX;
	int m_LastMimicY;
	void Cleanup();
	HRESULT Mimic(CAdWaypointGroup *Wpt, float Scale = 100.0f, int X = 0, int Y = 0);
	DECLARE_PERSISTENT(CAdWaypointGroup, CBObject)
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	bool m_Active;
	CAdWaypointGroup(CBGame *inGame);
	HRESULT LoadFile(char *Filename);
	HRESULT LoadBuffer(byte  *Buffer, bool Complete = true);
	virtual ~CAdWaypointGroup();
	CBArray<CBPoint *, CBPoint *> m_Points;
	int m_EditorSelectedPoint;
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
};

} // end of namespace WinterMute

#endif
