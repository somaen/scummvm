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

#include "engines/wintermute/dcgf.h"
#include "engines/wintermute/BParser.h"
#include "engines/wintermute/BSubFrame.h"
#include "engines/wintermute/BActiveRect.h"
#include "engines/wintermute/BDynBuffer.h"
#include "engines/wintermute/BSurface.h"
#include "engines/wintermute/BSurfaceStorage.h"
#include "engines/wintermute/BGame.h"
#include "engines/wintermute/PlatformSDL.h"
#include "engines/wintermute/scriptables/ScValue.h"
#include "engines/wintermute/scriptables/ScStack.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CBSubFrame, false)

//////////////////////////////////////////////////////////////////////////
CBSubFrame::CBSubFrame(CBGame *inGame): CBScriptable(inGame, true) {
	_surface = NULL;
	_hotspotX = _hotspotY = 0;
	_alpha = 0xFFFFFFFF;
	_transparent = 0xFFFF00FF;

	CBPlatform::SetRectEmpty(&_rect);

	_editorSelected = false;

	_surfaceFilename = NULL;
	_cKDefault = true;
	_cKRed = _cKBlue = _cKGreen = 0;
	_lifeTime = -1;
	_keepLoaded = false;

	_2DOnly = _3DOnly = false;
	_decoration = false;

	_mirrorX = _mirrorY = false;
}


//////////////////////////////////////////////////////////////////////////
CBSubFrame::~CBSubFrame() {
	if (_surface) Game->_surfaceStorage->RemoveSurface(_surface);
	delete[] _surfaceFilename;
	_surfaceFilename = NULL;
}


TOKEN_DEF_START
TOKEN_DEF(IMAGE)
TOKEN_DEF(TRANSPARENT)
TOKEN_DEF(RECT)
TOKEN_DEF(HOTSPOT)
TOKEN_DEF(2D_ONLY)
TOKEN_DEF(3D_ONLY)
TOKEN_DEF(DECORATION)
TOKEN_DEF(ALPHA_COLOR)
TOKEN_DEF(ALPHA)
TOKEN_DEF(MIRROR_X)
TOKEN_DEF(MIRROR_Y)
TOKEN_DEF(EDITOR_SELECTED)
TOKEN_DEF(EDITOR_PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::LoadBuffer(byte  *Buffer, int LifeTime, bool KeepLoaded) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(IMAGE)
	TOKEN_TABLE(TRANSPARENT)
	TOKEN_TABLE(RECT)
	TOKEN_TABLE(HOTSPOT)
	TOKEN_TABLE(2D_ONLY)
	TOKEN_TABLE(3D_ONLY)
	TOKEN_TABLE(DECORATION)
	TOKEN_TABLE(ALPHA_COLOR)
	TOKEN_TABLE(ALPHA)
	TOKEN_TABLE(MIRROR_X)
	TOKEN_TABLE(MIRROR_Y)
	TOKEN_TABLE(EDITOR_SELECTED)
	TOKEN_TABLE(EDITOR_PROPERTY)
	TOKEN_TABLE_END

	char *params;
	int cmd;
	CBParser parser(Game);
	RECT rect;
	int r = 255, g = 255, b = 255;
	int ar = 255, ag = 255, ab = 255, alpha = 255;
	bool custo_trans = false;
	CBPlatform::SetRectEmpty(&rect);
	char *surface_file = NULL;

	delete _surface;
	_surface = NULL;

	while ((cmd = parser.GetCommand((char **)&Buffer, commands, &params)) > 0) {
		switch (cmd) {
		case TOKEN_IMAGE:
			surface_file = params;
			break;

		case TOKEN_TRANSPARENT:
			parser.ScanStr(params, "%d,%d,%d", &r, &g, &b);
			custo_trans = true;
			break;

		case TOKEN_RECT:
			parser.ScanStr(params, "%d,%d,%d,%d", &rect.left, &rect.top, &rect.right, &rect.bottom);
			break;

		case TOKEN_HOTSPOT:
			parser.ScanStr(params, "%d,%d", &_hotspotX, &_hotspotY);
			break;

		case TOKEN_2D_ONLY:
			parser.ScanStr(params, "%b", &_2DOnly);
			break;

		case TOKEN_3D_ONLY:
			parser.ScanStr(params, "%b", &_3DOnly);
			break;

		case TOKEN_MIRROR_X:
			parser.ScanStr(params, "%b", &_mirrorX);
			break;

		case TOKEN_MIRROR_Y:
			parser.ScanStr(params, "%b", &_mirrorY);
			break;

		case TOKEN_DECORATION:
			parser.ScanStr(params, "%b", &_decoration);
			break;

		case TOKEN_ALPHA_COLOR:
			parser.ScanStr(params, "%d,%d,%d", &ar, &ag, &ab);
			break;

		case TOKEN_ALPHA:
			parser.ScanStr(params, "%d", &alpha);
			break;

		case TOKEN_EDITOR_SELECTED:
			parser.ScanStr(params, "%b", &_editorSelected);
			break;

		case TOKEN_EDITOR_PROPERTY:
			ParseEditorProperty((byte  *)params, false);
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND) {
		Game->LOG(0, "Syntax error in SUBFRAME definition");
		return E_FAIL;
	}

	if (surface_file != NULL) {
		if (custo_trans) SetSurface(surface_file, false, r, g, b, LifeTime, KeepLoaded);
		else SetSurface(surface_file, true, 0, 0, 0, LifeTime, KeepLoaded);
	}

	_alpha = DRGBA(ar, ag, ab, alpha);
	if (custo_trans) _transparent = DRGBA(r, g, b, 0xFF);

	/*
	if(_surface == NULL)
	{
	    Game->LOG(0, "Error parsing sub-frame. Image not set.");
	    return E_FAIL;
	}
	*/
	if (CBPlatform::IsRectEmpty(&rect)) SetDefaultRect();
	else _rect = rect;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::Draw(int X, int Y, CBObject *Register, float ZoomX, float ZoomY, bool Precise, uint32 Alpha, float Rotate, TSpriteBlendMode BlendMode) {
	if (!_surface) return S_OK;

	if (Register != NULL && !_decoration) {
		if (ZoomX == 100 && ZoomY == 100) {
			Game->_renderer->_rectList.Add(new CBActiveRect(Game, Register, this, X - _hotspotX + _rect.left, Y - _hotspotY + _rect.top, _rect.right - _rect.left, _rect.bottom - _rect.top, ZoomX, ZoomY, Precise));
		} else {
			Game->_renderer->_rectList.Add(new CBActiveRect(Game, Register, this, (int)(X - (_hotspotX + _rect.left) * (ZoomX / 100)), (int)(Y - (_hotspotY + _rect.top) * (ZoomY / 100)), (int)((_rect.right - _rect.left) * (ZoomX / 100)), (int)((_rect.bottom - _rect.top) * (ZoomY / 100)), ZoomX, ZoomY, Precise));
		}
	}
	if (Game->_suspendedRendering) return S_OK;

	HRESULT res;

	//if(Alpha==0xFFFFFFFF) Alpha = _alpha; // TODO: better (combine owner's and self alpha)
	if (_alpha != 0xFFFFFFFF) Alpha = _alpha;

	if (Rotate != 0.0f) {
		res = _surface->DisplayTransform((int)(X - _hotspotX * (ZoomX / 100)), (int)(Y - _hotspotY * (ZoomY / 100)), _hotspotX, _hotspotY, _rect, ZoomX, ZoomY, Alpha, Rotate, BlendMode, _mirrorX, _mirrorY);
	} else {
		if (ZoomX == 100 && ZoomY == 100) res = _surface->DisplayTrans(X - _hotspotX, Y - _hotspotY, _rect, Alpha, BlendMode, _mirrorX, _mirrorY);
		else res = _surface->DisplayTransZoom((int)(X - _hotspotX * (ZoomX / 100)), (int)(Y - _hotspotY * (ZoomY / 100)), _rect, ZoomX, ZoomY, Alpha, BlendMode, _mirrorX, _mirrorY);
	}

	return res;
}


//////////////////////////////////////////////////////////////////////////
bool CBSubFrame::GetBoundingRect(LPRECT Rect, int X, int Y, float ScaleX, float ScaleY) {
	if (!Rect) return false;

	float RatioX = ScaleX / 100.0f;
	float RatioY = ScaleY / 100.0f;

	CBPlatform::SetRect(Rect,
	                    X - _hotspotX * RatioX,
	                    Y - _hotspotY * RatioY,
	                    X - _hotspotX * RatioX + (_rect.right - _rect.left)*RatioX,
	                    Y - _hotspotY * RatioY + (_rect.bottom - _rect.top)*RatioY);
	return true;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::SaveAsText(CBDynBuffer *Buffer, int Indent, bool Complete) {
	if (Complete)
		Buffer->PutTextIndent(Indent, "SUBFRAME {\n");

	if (_surface && _surface->_filename != NULL)
		Buffer->PutTextIndent(Indent + 2, "IMAGE = \"%s\"\n", _surface->_filename);

	if (_transparent != 0xFFFF00FF)
		Buffer->PutTextIndent(Indent + 2, "TRANSPARENT { %d,%d,%d }\n", D3DCOLGetR(_transparent), D3DCOLGetG(_transparent), D3DCOLGetB(_transparent));

	RECT rect;
	CBPlatform::SetRectEmpty(&rect);
	if (_surface) CBPlatform::SetRect(&rect, 0, 0, _surface->GetWidth(), _surface->GetHeight());
	if (!CBPlatform::EqualRect(&rect, &_rect))
		Buffer->PutTextIndent(Indent + 2, "RECT { %d,%d,%d,%d }\n", _rect.left, _rect.top, _rect.right, _rect.bottom);

	if (_hotspotX != 0 || _hotspotY != 0)
		Buffer->PutTextIndent(Indent + 2, "HOTSPOT {%d, %d}\n", _hotspotX, _hotspotY);

	if (_alpha != 0xFFFFFFFF) {
		Buffer->PutTextIndent(Indent + 2, "ALPHA_COLOR { %d,%d,%d }\n", D3DCOLGetR(_alpha), D3DCOLGetG(_alpha), D3DCOLGetB(_alpha));
		Buffer->PutTextIndent(Indent + 2, "ALPHA = %d\n", D3DCOLGetA(_alpha));
	}

	if (_mirrorX)
		Buffer->PutTextIndent(Indent + 2, "MIRROR_X=%s\n", _mirrorX ? "TRUE" : "FALSE");

	if (_mirrorY)
		Buffer->PutTextIndent(Indent + 2, "MIRROR_Y=%s\n", _mirrorY ? "TRUE" : "FALSE");

	if (_2DOnly)
		Buffer->PutTextIndent(Indent + 2, "2D_ONLY=%s\n", _2DOnly ? "TRUE" : "FALSE");

	if (_3DOnly)
		Buffer->PutTextIndent(Indent + 2, "3D_ONLY=%s\n", _3DOnly ? "TRUE" : "FALSE");

	if (_decoration)
		Buffer->PutTextIndent(Indent + 2, "DECORATION=%s\n", _decoration ? "TRUE" : "FALSE");

	if (_editorSelected)
		Buffer->PutTextIndent(Indent + 2, "EDITOR_SELECTED=%s\n", _editorSelected ? "TRUE" : "FALSE");

	CBBase::SaveAsText(Buffer, Indent + 2);


	if (Complete)
		Buffer->PutTextIndent(Indent, "}\n\n");

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CBSubFrame::SetDefaultRect() {
	if (_surface) {
		CBPlatform::SetRect(&_rect, 0, 0, _surface->GetWidth(), _surface->GetHeight());
	} else CBPlatform::SetRectEmpty(&_rect);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::Persist(CBPersistMgr *PersistMgr) {

	CBScriptable::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(_2DOnly));
	PersistMgr->Transfer(TMEMBER(_3DOnly));
	PersistMgr->Transfer(TMEMBER(_alpha));
	PersistMgr->Transfer(TMEMBER(_decoration));
	PersistMgr->Transfer(TMEMBER(_editorSelected));
	PersistMgr->Transfer(TMEMBER(_hotspotX));
	PersistMgr->Transfer(TMEMBER(_hotspotY));
	PersistMgr->Transfer(TMEMBER(_rect));

	PersistMgr->Transfer(TMEMBER(_surfaceFilename));
	PersistMgr->Transfer(TMEMBER(_cKDefault));
	PersistMgr->Transfer(TMEMBER(_cKRed));
	PersistMgr->Transfer(TMEMBER(_cKGreen));
	PersistMgr->Transfer(TMEMBER(_cKBlue));
	PersistMgr->Transfer(TMEMBER(_lifeTime));

	PersistMgr->Transfer(TMEMBER(_keepLoaded));
	PersistMgr->Transfer(TMEMBER(_mirrorX));
	PersistMgr->Transfer(TMEMBER(_mirrorY));
	PersistMgr->Transfer(TMEMBER(_transparent));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, const char *Name) {

	//////////////////////////////////////////////////////////////////////////
	// GetImage
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "GetImage") == 0) {
		Stack->CorrectParams(0);

		if (!_surfaceFilename) Stack->PushNULL();
		else Stack->PushString(_surfaceFilename);
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// SetImage
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "SetImage") == 0) {
		Stack->CorrectParams(1);
		CScValue *Val = Stack->Pop();

		if (Val->IsNULL()) {
			if (_surface) Game->_surfaceStorage->RemoveSurface(_surface);
			delete[] _surfaceFilename;
			_surfaceFilename = NULL;
			Stack->PushBool(true);
		} else {
			const char *Filename = Val->GetString();
			if (SUCCEEDED(SetSurface(Filename))) {
				SetDefaultRect();
				Stack->PushBool(true);
			} else Stack->PushBool(false);
		}

		return S_OK;
	}

	else return CBScriptable::ScCallMethod(Script, Stack, ThisStack, Name);
}


//////////////////////////////////////////////////////////////////////////
CScValue *CBSubFrame::ScGetProperty(const char *Name) {
	if (!_scValue) _scValue = new CScValue(Game);
	_scValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type (RO)
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Type") == 0) {
		_scValue->SetString("subframe");
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// AlphaColor
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AlphaColor") == 0) {

		_scValue->SetInt((int)_alpha);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// TransparentColor (RO)
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "TransparentColor") == 0) {
		_scValue->SetInt((int)_transparent);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Is2DOnly
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Is2DOnly") == 0) {
		_scValue->SetBool(_2DOnly);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Is3DOnly
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Is3DOnly") == 0) {
		_scValue->SetBool(_3DOnly);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// MirrorX
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MirrorX") == 0) {
		_scValue->SetBool(_mirrorX);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// MirrorY
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MirrorY") == 0) {
		_scValue->SetBool(_mirrorY);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Decoration
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Decoration") == 0) {
		_scValue->SetBool(_decoration);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// HotspotX
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "HotspotX") == 0) {
		_scValue->SetInt(_hotspotX);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// HotspotY
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "HotspotY") == 0) {
		_scValue->SetInt(_hotspotY);
		return _scValue;
	}

	else return CBScriptable::ScGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::ScSetProperty(const char *Name, CScValue *Value) {
	//////////////////////////////////////////////////////////////////////////
	// AlphaColor
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "AlphaColor") == 0) {
		_alpha = (uint32)Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Is2DOnly
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Is2DOnly") == 0) {
		_2DOnly = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Is3DOnly
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Is3DOnly") == 0) {
		_3DOnly = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// MirrorX
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MirrorX") == 0) {
		_mirrorX = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// MirrorY
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MirrorY") == 0) {
		_mirrorY = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Decoration
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Decoration") == 0) {
		_decoration = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// HotspotX
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "HotspotX") == 0) {
		_hotspotX = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// HotspotY
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "HotspotY") == 0) {
		_hotspotY = Value->GetInt();
		return S_OK;
	}

	else return CBScriptable::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char *CBSubFrame::ScToString() {
	return "[subframe]";
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::SetSurface(const char *Filename, bool default_ck, byte ck_red, byte ck_green, byte ck_blue, int LifeTime, bool KeepLoaded) {
	if (_surface) {
		Game->_surfaceStorage->RemoveSurface(_surface);
		_surface = NULL;
	}

	delete[] _surfaceFilename;
	_surfaceFilename = NULL;

	_surface = Game->_surfaceStorage->AddSurface(Filename, default_ck, ck_red, ck_green, ck_blue, LifeTime, KeepLoaded);
	if (_surface) {
		_surfaceFilename = new char[strlen(Filename) + 1];
		strcpy(_surfaceFilename, Filename);

		_cKDefault = default_ck;
		_cKRed = ck_red;
		_cKGreen = ck_green;
		_cKBlue = ck_blue;
		_lifeTime = LifeTime;
		_keepLoaded = KeepLoaded;

		return S_OK;
	} else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSubFrame::SetSurfaceSimple() {
	if (!_surfaceFilename) {
		_surface = NULL;
		return S_OK;
	}
	_surface = Game->_surfaceStorage->AddSurface(_surfaceFilename, _cKDefault, _cKRed, _cKGreen, _cKBlue, _lifeTime, _keepLoaded);
	if (_surface) return S_OK;
	else return E_FAIL;
}

} // end of namespace WinterMute
