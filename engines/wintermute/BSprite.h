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

#ifndef WINTERMUTE_BSPRITE_H
#define WINTERMUTE_BSPRITE_H


#include "engines/wintermute/coll_templ.h"
#include "engines/wintermute/BScriptHolder.h"

namespace WinterMute {
class CBFrame;
class CBSurface;
class CBObject;
class CBSprite: public CBScriptHolder {
public:
	HRESULT KillAllSounds();
	CBSurface *GetSurface();
	char *_editorBgFile;
	int _editorBgOffsetX;
	int _editorBgOffsetY;
	int _editorBgAlpha;
	bool _streamed;
	bool _streamedKeepLoaded;
	void Cleanup();
	void SetDefaults();
	bool _precise;
	DECLARE_PERSISTENT(CBSprite, CBScriptHolder)

	bool _editorAllFrames;
	bool GetBoundingRect(LPRECT Rect, int X, int Y, float ScaleX = 100, float ScaleY = 100);
	int _moveY;
	int _moveX;
	HRESULT Display(int X, int Y, CBObject *Register = NULL, float ZoomX = 100, float ZoomY = 100, uint32 Alpha = 0xFFFFFFFF, float Rotate = 0.0f, TSpriteBlendMode BlendMode = BLEND_NORMAL);
	bool GetCurrentFrame(float ZoomX = 100, float ZoomY = 100);
	bool _canBreak;
	bool _editorMuted;
	bool _continuous;
	void Reset();
	CBObject *_owner;
	bool _changed;
	bool _paused;
	bool _finished;
	HRESULT LoadBuffer(byte  *Buffer, bool Compete = true, int LifeTime = -1, TSpriteCacheType CacheType = CACHE_ALL);
	HRESULT LoadFile(char *Filename, int LifeTime = -1, TSpriteCacheType CacheType = CACHE_ALL);
	uint32 _lastFrameTime;
	HRESULT Draw(int X, int Y, CBObject *Register = NULL, float ZoomX = 100, float ZoomY = 100, uint32 Alpha = 0xFFFFFFFF);
	bool _looping;
	int _currentFrame;
	HRESULT AddFrame(char *Filename, uint32 Delay = 0, int HotspotX = 0, int HotspotY = 0, RECT *Rect = NULL);
	CBSprite(CBGame *inGame, CBObject *Owner = NULL);
	virtual ~CBSprite();
	CBArray<CBFrame *, CBFrame *> _frames;
	HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);

	// scripting interface
	virtual CScValue *ScGetProperty(char *Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char *ScToString();
};

} // end of namespace WinterMute

#endif
