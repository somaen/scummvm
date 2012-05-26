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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "common/algorithm.h"
#include "common/endian.h"
#include "common/util.h"
#include "common/rect.h"
#include "common/textconsole.h"
#include "graphics/primitives.h"
#include "graphics/transparentSurface.h"

namespace Graphics {

bool RenderedImage::blit(int posX, int posY, int flipping, Common::Rect *pPartRect, uint color, int width, int height) {
	int ca = (color >> 24) & 0xff;

	// Check if we need to draw anything at all
	if (ca == 0)
		return true;

	int cr = (color >> 16) & 0xff;
	int cg = (color >> 8) & 0xff;
	int cb = (color >> 0) & 0xff;

	// Compensate for transparency. Since we're coming
	// down to 255 alpha, we just compensate for the colors here
	if (ca != 255) {
		cr = cr * ca >> 8;
		cg = cg * ca >> 8;
		cb = cb * ca >> 8;
	}

	// Create an encapsulating surface for the data
	Graphics::Surface srcImage;
	// TODO: Is the data really in the screen format?
	srcImage.format = g_system->getScreenFormat();
	srcImage.pitch = _width * 4;
	srcImage.w = _width;
	srcImage.h = _height;
	srcImage.pixels = _data;

	if (pPartRect) {
		srcImage.pixels = &_data[pPartRect->top * srcImage.pitch + pPartRect->left * 4];
		srcImage.w = pPartRect->right - pPartRect->left;
		srcImage.h = pPartRect->bottom - pPartRect->top;

		debug(6, "Blit(%d, %d, %d, [%d, %d, %d, %d], %08x, %d, %d)", posX, posY, flipping,
		      pPartRect->left,  pPartRect->top, pPartRect->width(), pPartRect->height(), color, width, height);
	} else {

		debug(6, "Blit(%d, %d, %d, [%d, %d, %d, %d], %08x, %d, %d)", posX, posY, flipping, 0, 0,
		      srcImage.w, srcImage.h, color, width, height);
	}

	if (width == -1)
		width = srcImage.w;
	if (height == -1)
		height = srcImage.h;

#ifdef SCALING_TESTING
	// Hardcode scaling to 66% to test scaling
	width = width * 2 / 3;
	height = height * 2 / 3;
#endif

	Graphics::Surface *img;
	Graphics::Surface *imgScaled = NULL;
	byte *savedPixels = NULL;
	if ((width != srcImage.w) || (height != srcImage.h)) {
		// Scale the image
		img = imgScaled = scale(srcImage, width, height);
		savedPixels = (byte *)img->pixels;
	} else {
		img = &srcImage;
	}

	// Handle off-screen clipping
	if (posY < 0) {
		img->h = MAX(0, (int)img->h - -posY);
		img->pixels = (byte *)img->pixels + img->pitch * -posY;
		posY = 0;
	}

	if (posX < 0) {
		img->w = MAX(0, (int)img->w - -posX);
		img->pixels = (byte *)img->pixels + (-posX * 4);
		posX = 0;
	}

	img->w = CLIP((int)img->w, 0, (int)MAX((int)_backSurface->w - posX, 0));
	img->h = CLIP((int)img->h, 0, (int)MAX((int)_backSurface->h - posY, 0));

	if ((img->w > 0) && (img->h > 0)) {
		int xp = 0, yp = 0;

		int inStep = 4;
		int inoStep = img->pitch;
		if (flipping & Image::FLIP_V) {
			inStep = -inStep;
			xp = img->w - 1;
		}

		if (flipping & Image::FLIP_H) {
			inoStep = -inoStep;
			yp = img->h - 1;
		}

		byte *ino = (byte *)img->getBasePtr(xp, yp);
		byte *outo = (byte *)_backSurface->getBasePtr(posX, posY);
		byte *in, *out;

		for (int i = 0; i < img->h; i++) {
			out = outo;
			in = ino;
			for (int j = 0; j < img->w; j++) {
				uint32 pix = *(uint32 *)in;
				int b = (pix >> 0) & 0xff;
				int g = (pix >> 8) & 0xff;
				int r = (pix >> 16) & 0xff;
				int a = (pix >> 24) & 0xff;
				in += inStep;

				if (ca != 255) {
					a = a * ca >> 8;
				}

				switch (a) {
				case 0: // Full transparency
					out += 4;
					break;
				case 255: // Full opacity
#if defined(SCUMM_LITTLE_ENDIAN)
					if (cb != 255)
						*out++ = (b * cb) >> 8;
					else
						*out++ = b;

					if (cg != 255)
						*out++ = (g * cg) >> 8;
					else
						*out++ = g;

					if (cr != 255)
						*out++ = (r * cr) >> 8;
					else
						*out++ = r;

					*out++ = a;
#else
					*out++ = a;

					if (cr != 255)
						*out++ = (r * cr) >> 8;
					else
						*out++ = r;

					if (cg != 255)
						*out++ = (g * cg) >> 8;
					else
						*out++ = g;

					if (cb != 255)
						*out++ = (b * cb) >> 8;
					else
						*out++ = b;
#endif
					break;

				default: // alpha blending
#if defined(SCUMM_LITTLE_ENDIAN)
					if (cb == 0)
						*out = 0;
					else if (cb != 255)
						*out += ((b - *out) * a * cb) >> 16;
					else
						*out += ((b - *out) * a) >> 8;
					out++;
					if (cg == 0)
						*out = 0;
					else if (cg != 255)
						*out += ((g - *out) * a * cg) >> 16;
					else
						*out += ((g - *out) * a) >> 8;
					out++;
					if (cr == 0)
						*out = 0;
					else if (cr != 255)
						*out += ((r - *out) * a * cr) >> 16;
					else
						*out += ((r - *out) * a) >> 8;
					out++;
					*out = 255;
					out++;
#else
					*out = 255;
					out++;
					if (cr == 0)
						*out = 0;
					else if (cr != 255)
						*out += ((r - *out) * a * cr) >> 16;
					else
						*out += ((r - *out) * a) >> 8;
					out++;
					if (cg == 0)
						*out = 0;
					else if (cg != 255)
						*out += ((g - *out) * a * cg) >> 16;
					else
						*out += ((g - *out) * a) >> 8;
					out++;
					if (cb == 0)
						*out = 0;
					else if (cb != 255)
						*out += ((b - *out) * a * cb) >> 16;
					else
						*out += ((b - *out) * a) >> 8;
					out++;
#endif
				}
			}
			outo += _backSurface->pitch;
			ino += inoStep;
		}

		g_system->copyRectToScreen((byte *)_backSurface->getBasePtr(posX, posY), _backSurface->pitch, posX, posY,
		                           img->w, img->h);
	}

	if (imgScaled) {
		imgScaled->pixels = savedPixels;
		imgScaled->free();
		delete imgScaled;
	}

	return true;
}

/**
 * Scales a passed surface, creating a new surface with the result
 * @param srcImage      Source image to scale
 * @param scaleFactor   Scale amount. Must be between 0 and 1.0 (but not zero)
 * @remarks Caller is responsible for freeing the returned surface
 */
Graphics::Surface *RenderedImage::scale(const Graphics::Surface &srcImage, int xSize, int ySize) {
	Graphics::Surface *s = new Graphics::Surface();
	s->create(xSize, ySize, srcImage.format);

	int *horizUsage = scaleLine(xSize, srcImage.w);
	int *vertUsage = scaleLine(ySize, srcImage.h);

	// Loop to create scaled version
	for (int yp = 0; yp < ySize; ++yp) {
		const byte *srcP = (const byte *)srcImage.getBasePtr(0, vertUsage[yp]);
		byte *destP = (byte *)s->getBasePtr(0, yp);

		for (int xp = 0; xp < xSize; ++xp) {
			const byte *tempSrcP = srcP + (horizUsage[xp] * srcImage.format.bytesPerPixel);
			for (int byteCtr = 0; byteCtr < srcImage.format.bytesPerPixel; ++byteCtr) {
				*destP++ = *tempSrcP++;
			}
		}
	}

	// Delete arrays and return surface
	delete[] horizUsage;
	delete[] vertUsage;
	return s;
}

/**
 * Returns an array indicating which pixels of a source image horizontally or vertically get
 * included in a scaled image
 */
int *RenderedImage::scaleLine(int size, int srcSize) {
	int scale = 100 * size / srcSize;
	assert(scale > 0);
	int *v = new int[size];
	Common::fill(v, &v[size], 0);

	int distCtr = 0;
	int *destP = v;
	for (int distIndex = 0; distIndex < srcSize; ++distIndex) {
		distCtr += scale;
		while (distCtr >= 100) {
			assert(destP < &v[size]);
			*destP++ = distIndex;
			distCtr -= 100;
		}
	}

	return v;
}


} // End of namespace Graphics
