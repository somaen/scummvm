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

#ifndef WINTERMUTE_BSURFACE_H
#define WINTERMUTE_BSURFACE_H

#include "BBase.h"

namespace WinterMute {

class CBSurface: public CBBase {
public:
	virtual HRESULT Invalidate();
	virtual HRESULT PrepareToDraw();
	bool m_CKDefault;
	byte m_CKRed;
	byte m_CKGreen;
	byte m_CKBlue;

	uint32 m_LastUsedTime;
	bool m_Valid;
	int m_LifeTime;
	bool m_KeepLoaded;

	bool m_PixelOpReady;
	CBSurface(CBGame *inGame);
	virtual ~CBSurface();

	virtual HRESULT DisplayHalfTrans(int X, int Y, RECT rect);
	virtual bool IsTransparentAt(int X, int Y);
	virtual HRESULT DisplayTransZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, uint32 Alpha = 0xFFFFFFFF, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false);
	virtual HRESULT DisplayTrans(int X, int Y, RECT rect, uint32 Alpha = 0xFFFFFFFF, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false);
	virtual HRESULT DisplayTransOffset(int X, int Y, RECT rect, uint32 Alpha = 0xFFFFFFFF, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false, int offsetX = 0, int offsetY = 0);
	virtual HRESULT Display(int X, int Y, RECT rect, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false);
	virtual HRESULT DisplayZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, uint32 Alpha = 0xFFFFFFFF, bool Transparent = false, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false);
	virtual HRESULT DisplayTransform(int X, int Y, int HotX, int HotY, RECT Rect, float ZoomX, float ZoomY, uint32 Alpha, float Rotate, TSpriteBlendMode BlendMode = BLEND_NORMAL, bool MirrorX = false, bool MirrorY = false);
	virtual HRESULT Restore();
	virtual HRESULT Create(char *Filename, bool default_ck, byte ck_red, byte ck_green, byte ck_blue, int LifeTime = -1, bool KeepLoaded = false);
	virtual HRESULT Create(int Width, int Height);
	virtual HRESULT PutPixel(int X, int Y, byte R, byte G, byte B, int A = -1);
	virtual HRESULT GetPixel(int X, int Y, byte *R, byte *G, byte *B, byte *A = NULL);
	virtual bool ComparePixel(int X, int Y, byte R, byte G, byte B, int A = -1);
	virtual HRESULT StartPixelOp();
	virtual HRESULT EndPixelOp();
	virtual bool IsTransparentAtLite(int X, int Y);
	void SetFilename(const char *Filename);
	void SetSize(int Width, int Height);

	int m_ReferenceCount;
	char *m_Filename;

	int GetWidth() {
		return m_Width;
	}
	int GetHeight() {
		return m_Height;
	}
	//void SetWidth(int Width){ m_Width = Width;    }
	//void SetHeight(int Height){ m_Height = Height; }
protected:
	int m_Height;
	int m_Width;

};

} // end of namespace WinterMute

#endif
