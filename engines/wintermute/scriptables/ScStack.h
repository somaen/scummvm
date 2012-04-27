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

#ifndef WINTERMUTE_SCSTACK_H
#define WINTERMUTE_SCSTACK_H


#include "engines/wintermute/BBase.h"
#include "engines/wintermute/coll_templ.h"
#include "engines/wintermute/persistent.h"

namespace WinterMute {

class CScValue;
class CBScriptable;

class CScStack : public CBBase {
public:
	CScValue *GetAt(int Index);
	CScValue *GetPushValue();
	DECLARE_PERSISTENT(CScStack, CBBase)
	void PushNative(CBScriptable *Val, bool Persistent);
	void PushString(const char *Val);
	void PushBool(bool Val);
	void PushInt(int Val);
	void PushFloat(double Val);
	void PushNULL();
	void CorrectParams(uint32 expected_params);
	CScValue *GetTop();
	void Push(CScValue *Val);
	CScValue *Pop();
	CScStack(CBGame *inGame);
	virtual ~CScStack();
	CBArray<CScValue *, CScValue *> _values;
	int _sP;

};

} // end of namespace WinterMute

#endif
