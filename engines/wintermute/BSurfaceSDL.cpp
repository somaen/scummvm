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

#include "dcgf.h"
#include "BFile.h"
#include "BGame.h"
#include "BSurfaceSDL.h"
#include "BRenderSDL.h"
//#include "SdlUtil.h"
#include "graphics/decoders/png.h"
#include "graphics/decoders/bmp.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"
#include "common/stream.h"
#include "BFileManager.h"
#include "PlatformSDL.h"

namespace WinterMute {

//////////////////////////////////////////////////////////////////////////
CBSurfaceSDL::CBSurfaceSDL(CBGame *inGame) : CBSurface(inGame) {
	_surface = new Graphics::Surface();
	_alphaMask = NULL;

	_lockPixels = NULL;
	_lockPitch = 0;
}

//////////////////////////////////////////////////////////////////////////
CBSurfaceSDL::~CBSurfaceSDL() {
	//TODO
	delete _surface;
#if 0
	if (_texture) SDL_DestroyTexture(_texture);
	delete[] _alphaMask;
	_alphaMask = NULL;

	Game->AddMem(-_width * _height * 4);
#endif
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Create(const char *Filename, bool default_ck, byte ck_red, byte ck_green, byte ck_blue, int LifeTime, bool KeepLoaded) {
/*	CBRenderSDL *renderer = static_cast<CBRenderSDL *>(Game->_renderer); */
	Common::String strFileName(Filename);

	Graphics::ImageDecoder *imgDecoder;

	if (strFileName.hasSuffix(".png")) {
		imgDecoder = new Graphics::PNGDecoder();
	} else if (strFileName.hasSuffix(".bmp")) {
		imgDecoder = new Graphics::BitmapDecoder();
	} else {
		error("CBSurfaceSDL::Create : Unsupported fileformat %s", Filename);
	}

	CBFile *file = Game->_fileManager->OpenFile(Filename);
	if (!file) return E_FAIL;

	imgDecoder->loadStream(*file->getMemStream());
	const Graphics::Surface *surface = imgDecoder->getSurface();
	Game->_fileManager->CloseFile(file);

	if (default_ck) {
		ck_red   = 255;
		ck_green = 0;
		ck_blue  = 255;
	}

	_width = surface->w;
	_height = surface->h;

	bool isSaveGameGrayscale = scumm_strnicmp(Filename, "savegame:", 9) == 0 && (Filename[strFileName.size() - 1] == 'g' || Filename[strFileName.size() - 1] == 'G');
	if (isSaveGameGrayscale) {
		warning("grayscaleConversion not yet implemented");
		/*      FIBITMAP *newImg = FreeImage_ConvertToGreyscale(img);
		        if (newImg) {
		            FreeImage_Unload(img);
		            img = newImg;
		        }*/
	}

	// convert 32-bit BMPs to 24-bit or they appear totally transparent (does any app actually write alpha in BMP properly?)
	/*  if (FreeImage_GetBPP(img) != 32 || (imgFormat == FIF_BMP && FreeImage_GetBPP(img) != 24)) {
	        FIBITMAP *newImg = FreeImage_ConvertTo24Bits(img);
	        if (newImg) {
	            FreeImage_Unload(img);
	            img = newImg;
	        } else {
	            FreeImage_Unload(img);
	            return -1;
	        }
	    }

	    FreeImage_FlipVertical(img);*/

	//TODO: This is rather endian-specific, but should be replaced by non-SDL-code anyhow:
/*	uint32 rmask = surface->format.rMax() << surface->format.rShift;
	uint32 gmask = surface->format.gMax() << surface->format.gShift;
	uint32 bmask = surface->format.bMax() << surface->format.bShift;
	uint32 amask = surface->format.aMax();*/

//	SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(surface->pixels, _width, _height, surface->format.bytesPerPixel * 8, surface->pitch, rmask, gmask, bmask, amask);

	// no alpha, set color key
	/*  if (surface->format.bytesPerPixel != 4)
	        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, ck_red, ck_green, ck_blue));*/
	_surface = new Graphics::Surface();
	_surface->copyFrom(*surface);

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); //TODO
	//_texture = SdlUtil::CreateTextureFromSurface(renderer->GetSdlRenderer(), surf);

	// This particular warning is rather messy, as this function is called a ton,
	// thus we avoid printing it more than once.
	static bool hasWarned = false;
	if (!hasWarned) {
		warning("Surface-textures not fully ported yet");
		hasWarned = true;
	}
#if 0
	_texture = SDL_CreateTextureFromSurface(renderer->GetSdlRenderer(), surf);
	if (!_texture) {
		SDL_FreeSurface(surf);
		delete imgDecoder;
		return E_FAIL;
	}

	GenAlphaMask(surf);

	SDL_FreeSurface(surf);
	delete imgDecoder; // TODO: Update this if ImageDecoder doesn't end up owning the surface.

	_cKDefault = default_ck;
	_cKRed = ck_red;
	_cKGreen = ck_green;
	_cKBlue = ck_blue;
#endif

	if (!_filename || scumm_stricmp(_filename, Filename) != 0) {
		SetFilename(Filename);
	}

	if (_lifeTime == 0 || LifeTime == -1 || LifeTime > _lifeTime)
		_lifeTime = LifeTime;

	_keepLoaded = KeepLoaded;
	if (_keepLoaded) _lifeTime = -1;

	_valid = true;
#if 0
	Game->AddMem(_width * _height * 4);
#endif

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSurfaceSDL::GenAlphaMask(Graphics::Surface *surface) {
	warning("CBSurfaceSDL::GenAlphaMask - Not ported yet");
	return;
#if 0
	delete[] _alphaMask;
	_alphaMask = NULL;
	if (!surface) return;

	SDL_LockSurface(surface);

	bool hasColorKey;
	Uint32 colorKey;
	Uint8 ckRed, ckGreen, ckBlue;
	/*  if (SDL_GetColorKey(surface, &colorKey) == 0) {
	        hasColorKey = true;
	        SDL_GetRGB(colorKey, surface->format, &ckRed, &ckGreen, &ckBlue);
	    } else hasColorKey = false;
	*/ //TODO
	_alphaMask = new byte[surface->w * surface->h];

	bool hasTransparency = false;
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			Uint32 pixel = GetPixel(surface, x, y);

			Uint8 r, g, b, a;
			SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			if (hasColorKey && r == ckRed && g == ckGreen && b == ckBlue)
				a = 0;

			_alphaMask[y * surface->w + x] = a;
			if (a < 255) hasTransparency = true;
		}
	}

	SDL_UnlockSurface(surface);

	if (!hasTransparency) {
		delete[] _alphaMask;
		_alphaMask = NULL;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
uint32 CBSurfaceSDL::GetPixel(Graphics::Surface *surface, int x, int y) {
	warning("CBSurfaceSDL::GetPixel - Not ported yet");
#if 0
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Create(int Width, int Height) {
	warning("SurfaceSDL::Create not ported yet"); //TODO
#if 0
	CBRenderSDL *renderer = static_cast<CBRenderSDL *>(Game->_renderer);
	_texture = SDL_CreateTexture(renderer->GetSdlRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);

	_width = Width;
	_height = Height;

	Game->AddMem(_width * _height * 4);

	_valid = true;
#endif
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::CreateFromSDLSurface(Graphics::Surface *surface) {
	warning("CBSurfaceSDL::CreateFromSDLSurface not ported yet"); //TODO
#if 0
	CBRenderSDL *renderer = static_cast<CBRenderSDL *>(Game->_renderer);
	_texture = SDL_CreateTextureFromSurface(renderer->GetSdlRenderer(), surface);

	_width = surface->w;
	_height = surface->h;

	Game->AddMem(_width * _height * 4);

	_valid = true;
#endif
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBSurfaceSDL::IsTransparentAt(int X, int Y) {
	// This particular warning is rather messy, as this function is called a ton,
	// thus we avoid printing it more than once.
	static bool hasWarned = false;
	if (!hasWarned) {
		warning("CBSurfaceSDL::IsTransparentAt not ported yet");
		hasWarned = true;
	}
#if 0
	int access;
	int width, height;
	//SDL_QueryTexture(_texture, NULL, &access, &width, &height); //TODO
	//if (access != SDL_TEXTUREACCESS_STREAMING) return false;
	if (X < 0 || X >= width || Y < 0 || Y >= height) return true;


	StartPixelOp();
	bool ret = IsTransparentAtLite(X, Y);
	EndPixelOp();

	return ret;
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool CBSurfaceSDL::IsTransparentAtLite(int X, int Y) {
	//if (!_lockPixels) return false;

	// This particular warning is rather messy, as this function is called a ton,
	// thus we avoid printing it more than once.
	static bool hasWarned = false;
	if (!hasWarned) {
		warning("CBSurfaceSDL::IsTransparentAtLite not ported yet");
		hasWarned = true;
	}
#if 0
	uint32 format;
	int access;
	int width, height;

	//SDL_QueryTexture(_texture, &format, &access, &width, &height);
	//if (access != SDL_TEXTUREACCESS_STREAMING) return false;
	if (X < 0 || X >= width || Y < 0 || Y >= height) return true;

	if (!_alphaMask) return false;
	else return _alphaMask[Y * width + X] <= 128;
#endif
	return false;
	/*
	Uint32* dst = (Uint32*)((Uint8*)_lockPixels + Y * _lockPitch);
	Uint32 pixel = dst[X];

	SDL_PixelFormat* pixelFormat = SDL_AllocFormat(format);
	Uint8 r, g, b, a;
	SDL_GetRGBA(pixel, pixelFormat, &r, &g, &b, &a);
	SDL_FreeFormat(pixelFormat);

	return a <= 128;
	*/
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::StartPixelOp() {
	//SDL_LockTexture(_texture, NULL, &_lockPixels, &_lockPitch);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::EndPixelOp() {
	//SDL_UnlockTexture(_texture);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Display(int X, int Y, RECT rect, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY) {
	return DrawSprite(X, Y, &rect, 100, 100, 0xFFFFFFFF, true, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTrans(int X, int Y, RECT rect, uint32 Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY) {
	return DrawSprite(X, Y, &rect, 100, 100, Alpha, false, BlendMode, MirrorX, MirrorY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransOffset(int X, int Y, RECT rect, uint32 Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY, int offsetX, int offsetY) {
	return DrawSprite(X, Y, &rect, 100, 100, Alpha, false, BlendMode, MirrorX, MirrorY, offsetX, offsetY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, uint32 Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY) {
	return DrawSprite(X, Y, &rect, ZoomX, ZoomY, Alpha, false, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, uint32 Alpha, bool Transparent, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY) {
	return DrawSprite(X, Y, &rect, ZoomX, ZoomY, Alpha, !Transparent, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransform(int X, int Y, int HotX, int HotY, RECT Rect, float ZoomX, float ZoomY, uint32 Alpha, float Rotate, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY) {
	return DrawSprite(X, Y, &Rect, ZoomX, ZoomY, Alpha, false, BlendMode, MirrorX, MirrorY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DrawSprite(int X, int Y, RECT *Rect, float ZoomX, float ZoomY, uint32 Alpha, bool AlphaDisable, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY, int offsetX, int offsetY) {
	CBRenderSDL *renderer = static_cast<CBRenderSDL *>(Game->_renderer);

	if (renderer->_forceAlphaColor != 0) Alpha = renderer->_forceAlphaColor;

	// This particular warning is rather messy, as this function is called a ton,
	// thus we avoid printing it more than once.
	static bool hasWarned = false;
	if (!hasWarned) {
		warning("CBSurfaceSDL::DrawSprite not fully ported yet"); // TODO.
		hasWarned = true;
	}
#if 0
	byte r = D3DCOLGetR(Alpha);
	byte g = D3DCOLGetG(Alpha);
	byte b = D3DCOLGetB(Alpha);
	byte a = D3DCOLGetA(Alpha);

	SDL_SetTextureColorMod(_texture, r, g, b);
	SDL_SetTextureAlphaMod(_texture, a);

	if (AlphaDisable)
		SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_NONE);
	else
		SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
#endif
	// TODO: This _might_ miss the intended behaviour by 1 in each direction
	// But I think it fits the model used in Wintermute.
	Common::Rect srcRect;
	srcRect.left = Rect->left;
	srcRect.top = Rect->top;
	srcRect.setWidth(Rect->right - Rect->left);
	srcRect.setHeight(Rect->bottom - Rect->top);

	Common::Rect position;
	position.left = X;
	position.top = Y;
	// TODO: Scaling...
	/*
	position.setWidth((float)srcRect.width() * ZoomX / 100.f);
	position.setHeight((float)srcRect.height() * ZoomX / 100.f);
	*/
	position.setWidth(srcRect.width());
	position.setHeight(srcRect.height());

	renderer->ModTargetRect(&position);

	position.left += offsetX;
	position.top += offsetY;

	renderer->drawFromSurface(_surface, &srcRect, &position);
#if 0
	SDL_RenderCopy(renderer->GetSdlRenderer(), _texture, &srcRect, &position);
#endif

	return S_OK;
}

} // end of namespace WinterMute
