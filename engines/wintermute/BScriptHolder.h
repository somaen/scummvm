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

#ifndef WINTERMUTE_BSCRIPTHOLDER_H
#define WINTERMUTE_BSCRIPTHOLDER_H

#include "coll_templ.h"
#include "persistent.h"
#include "BScriptable.h"

namespace WinterMute {

class CBScriptHolder : public CBScriptable {
public:
	DECLARE_PERSISTENT(CBScriptHolder, CBScriptable)
#if 0
	CBScriptHolder(CBGame *inGame);
	virtual ~CBScriptHolder();

	virtual CScScript *InvokeMethodThread(char *MethodName);
	virtual void MakeFreezable(bool Freezable);
	bool CanHandleEvent(char *EventName);
	virtual bool CanHandleMethod(char *EventMethod);
	HRESULT Cleanup();
	HRESULT RemoveScript(CScScript *Script);
	HRESULT AddScript(char *Filename);
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	virtual HRESULT Listen(CBScriptHolder *param1, uint32 param2);
	HRESULT ApplyEvent(const char *EventName, bool Unbreakable = false);
	void SetFilename(char *Filename);
	HRESULT ParseProperty(byte  *Buffer, bool Complete = true);

	char *m_Filename;
	bool m_Freezable;
	bool m_Ready;
	CBArray<CScScript *, CScScript *> m_Scripts;

	// scripting interface
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char *ScToString();
	virtual void ScDebuggerDesc(char *Buf, int BufSize);
#endif
	// IWmeObject
public:
	virtual bool SendEvent(const char *EventName);
};

} // end of namespace WinterMute

#endif
