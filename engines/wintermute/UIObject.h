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

#ifndef WINTERMUTE_UIOBJECT_H
#define WINTERMUTE_UIOBJECT_H


#include "BObject.h"
#include "dctypes.h"    // Added by ClassView

namespace WinterMute {

class CUITiledImage;
class CBFont;
class CUIObject : public CBObject {
public:

	HRESULT GetTotalOffset(int *OffsetX, int *OffsetY);
	bool m_CanFocus;
	HRESULT Focus();
	virtual HRESULT HandleMouse(TMouseEvent Event, TMouseButton Button);
	bool IsFocused();
	bool m_ParentNotify;
	DECLARE_PERSISTENT(CUIObject, CBObject)
	CUIObject *m_Parent;
	virtual HRESULT Display(int OffsetX = 0, int OffsetY = 0);
	virtual void CorrectSize();
	bool m_SharedFonts;
	bool m_SharedImages;
	void SetText(const char *Text);
	char *m_Text;
	CBFont *m_Font;
	bool m_Visible;
	CUITiledImage *m_Back;
	bool m_Disable;
	CUIObject(CBGame *inGame = NULL);
	virtual ~CUIObject();
	int m_Width;
	int m_Height;
	TUIObjectType m_Type;
	CBSprite *m_Image;
	void SetListener(CBScriptHolder *Object, CBScriptHolder *ListenerObject, uint32 ListenerParam);
	CBScriptHolder *m_ListenerParamObject;
	uint32 m_ListenerParamDWORD;
	CBScriptHolder *m_ListenerObject;
	CUIObject *m_FocusedWidget;
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);

	// scripting interface
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char *ScToString();
};

} // end of namespace WinterMute

#endif
