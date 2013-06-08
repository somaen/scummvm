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

#include "voyeur/graphics.h"
#include "voyeur/game.h"
#include "voyeur/voyeur.h"
#include "engines/util.h"
#include "graphics/palette.h"
#include "graphics/surface.h"

namespace Voyeur {

GraphicsManager::GraphicsManager() {
	_SVGAPage = 0;
	_SVGAMode = 0;
	_SVGAReset = 0;
	_screenOffset = 0;
	_planeSelect = 0;
	_sImageShift = 3;
	_palFlag = false;
	_MCGAMode = false;
	_saveBack = true;
	_clipPtr = NULL;
	_viewPortListPtr = NULL;
	_vPort = NULL;
}

void GraphicsManager::sInitGraphics() {
	initGraphics(SCREEN_WIDTH, SCREEN_HEIGHT, false);
	_screenSurface.create(SCREEN_WIDTH, SCREEN_HEIGHT,
		Graphics::PixelFormat::createFormatCLUT8());

	clearPalette();
}

GraphicsManager::~GraphicsManager() {
	_screenSurface.free();
}

void GraphicsManager::setupMCGASaveRect(ViewPortResource *viewPort) {
	_MCGAMode = true;

	if (viewPort->_activePage) {
		viewPort->_activePage->_flags |= 1;
		Common::Rect *clipRect = _clipPtr;
		_clipPtr = &viewPort->_clipRect;

		sDrawPic(viewPort->_activePage, viewPort->_currentPic, Common::Point());

		_clipPtr = clipRect;
	}

	viewPort->_rectListCount[1] = -1;
}

void GraphicsManager::addRectOptSaveRect(ViewPortResource *viewPort, int idx, const Common::Rect &bounds) {
	int count1, count2;
	int idx1, varE, var24;

	if (viewPort->_rectListCount[idx] == -1)
		return;

	viewPort->_rectListPtr[idx]->push_back(bounds);
	count1 = count2 = viewPort->_rectListCount[idx];
	varE = var24 = 0;

	if (count1 > 0) {
		for (idx1 = 0; idx1 < count1; ++idx1) {
			// TODO: In progress

			Common::Array<Common::Rect> &rectList = *viewPort->_rectListPtr[idx];
		}

		viewPort->_rectListCount[idx] = idx1;
	}
}

void GraphicsManager::restoreMCGASaveRect(ViewPortResource *viewPort) {
	if (viewPort->_rectListCount[0] != -1) {
		for (int i = 0; i < viewPort->_rectListCount[0]; ++i) {
			addRectOptSaveRect(viewPort, 1, (*viewPort->_rectListPtr[0])[i]);
		}
	} else {
		viewPort->_rectListCount[1] = -1;
	}

	restoreBack(*viewPort->_rectListPtr[1], viewPort->_rectListCount[1], viewPort->_pages[0],
		viewPort->_pages[1]);
	
	int count = viewPort->_rectListCount[0];
	restoreBack(*viewPort->_rectListPtr[0], viewPort->_rectListCount[0], 
		viewPort->_activePage, viewPort->_currentPic);

	SWAP(viewPort->_rectListPtr[0], viewPort->_rectListPtr[1]);
	viewPort->_rectListCount[1] = count;
}

void GraphicsManager::addRectNoSaveBack(ViewPortResource *viewPort, int idx, const Common::Rect &bounds) {
	// TODO: more
}

void GraphicsManager::sDrawPic(DisplayResource *srcDisplay, DisplayResource *destDisplay,
		const Common::Point &offset) {
	int var4C = 0;
	int width1, width2;
	int widthDiff, widthDiff2;
	int height1;
	int srcOffset;
	int screenOffset;
	int srcFlags, destFlags;
	ViewPortResource *destViewPort = NULL;
	Common::Rect newBounds;
	Common::Rect backBounds;
	int var24;
	bool isClipped = false;
	int var52;
	int var20, var22;
	int var26;
	byte *srcImgData, *destImgData;
	byte *srcP, *destP;

	// Get the picture parameters, or deference viewport pointers to get their pictures
	PictureResource *srcPic = (PictureResource *)srcDisplay;
	PictureResource *destPic = (PictureResource *)destDisplay;

	if (srcDisplay->_flags & 0x8000) {
		// A viewport was passed, not a picture
		srcPic = ((ViewPortResource *)srcDisplay)->_currentPic;
	}
	if (destDisplay->_flags & 0x8000) {
		destViewPort = (ViewPortResource *)destDisplay;
		destPic = destViewPort->_currentPic;
	}

	Common::Point ofs = Common::Point(offset.x + srcPic->_bounds.left - destPic->_bounds.left, 
		offset.y + srcPic->_bounds.top - destPic->_bounds.top);
	width1 = width2 = srcPic->_bounds.width();
	height1 = srcPic->_bounds.height();
	srcOffset = 0;
	srcFlags = srcPic->_flags;
	destFlags = destPic->_flags;

	if (srcFlags & 1) {
		if (_clipPtr) {
			int xs = _clipPtr->left - srcPic->_bounds.left;
			int ys = _clipPtr->top - srcPic->_bounds.top;
			newBounds = Common::Rect(xs, ys, xs + _clipPtr->width(), ys + _clipPtr->height());
		} else if (destViewPort) {
			int xs = destViewPort->_clipRect.left - destPic->_bounds.left;
			int ys = destViewPort->_clipRect.top - destPic->_bounds.top;
			newBounds = Common::Rect(xs, ys, xs + destViewPort->_clipRect.width(),
				ys + destViewPort->_clipRect.height());
		} else {
			newBounds = Common::Rect(0, 0, destPic->_bounds.width(), destPic->_bounds.height());
		}

		var24 = ofs.y - newBounds.top;
		if (var24 < 0) {
			var52 = width2;
			srcOffset -= var24 * var52;
			height1 += var24;
			ofs.y = newBounds.top;
			
			if (height1 <= 0)
				return;

			isClipped = true;
		}

		var20 = newBounds.bottom - (ofs.y + height1);
		if (var20 < 0) {
			height1 += var20;
			if (height1 <= 0)
				return;
		}

		var22 = ofs.x - newBounds.left;
		if (var22 < 0) {
			srcOffset -= var22;
			width2 += var22;
			ofs.x = newBounds.left;

			if (width2 <= 0)
				return;

			isClipped = true;
		}

		var26 = newBounds.right - (ofs.x + width2);
		if (var26 < 0) {
			width2 += var26;
			if (width2 <= 0)
				return;

			isClipped = true;
		}
	}

	screenOffset = ofs.y * destPic->_bounds.width() + ofs.x;
	widthDiff = width1 - width2;
	widthDiff2 = destPic->_bounds.width() - width2;

	if (destViewPort) {
		error("TODO: Examine further when it's actually used");
		if (!_saveBack || ((srcPic->_flags & 0x800) != 0)) {
			// TODO
		} else if (!destViewPort->_addFn) {
			// TODO
		} else {
			int xs = ofs.x + destPic->_bounds.left;
			int ys = ofs.y + destPic->_bounds.top;
			backBounds = Common::Rect(xs, ys, xs + width2, ys + height1);

			(this->*destViewPort->_addFn)(destViewPort, destViewPort->_bounds.top, backBounds);
		}
	}

	if (srcFlags & 0x1000) {
		srcImgData = srcPic->_imgData + (var4C << 14) + _screenOffset;
		for (uint idx = 0; idx < srcPic->_maskData; ++idx) {
			if (var4C < 4) {
				EMSMapPageHandle(srcPic->_planeSize, srcPic->_imgData[idx], var4C);
				++var4C;
			}
		}
	} else {
		srcImgData = srcPic->_imgData;
	}
	if (destFlags & 0x1000) {
		destImgData = destPic->_imgData + (var4C << 14) + _screenOffset;
		for (uint idx = 0; idx < srcPic->_maskData; ++idx) {
			if (var4C < 4) {
				EMSMapPageHandle(destPic->_planeSize, destPic->_imgData[idx], var4C);
				++var4C;
			}
		}		
	} else {
		destImgData = destPic->_imgData;
	}

	_SVGAPage = _SVGAReset;
	if (srcPic->_select != 0xff)
		return;

	if (srcPic->_pick == 0xff) {
		if (srcFlags & 8) {
			error("TODO: sDrawPic");
		} else {
			srcP = srcImgData + srcOffset;

			if (destFlags & 8) {
				// loc_258D8
				destP = destImgData + screenOffset;

				if (srcFlags & 2) {
					// loc_258F5f
				} else {
					// loc_25D40
					if (srcFlags & 0x100) {
						// loc_25D4A
					} else {
						// loc_2606D
						destP = (byte *)_screenSurface.pixels + screenOffset;

						for (int yp = 0; yp < height1; ++yp) {
							Common::copy(srcP, srcP + width2, destP);
							destP += width2 + widthDiff2;
							srcP += width2 + widthDiff;
						}
					}
				}
			} else {
				destP = destImgData + screenOffset;

				// loc_2615E
				if (srcFlags & 2) {
					error("TODO: sDrawPic");
				} else {
					if (srcFlags & 0x100) {
						srcP = srcImgData;

						if (isClipped) {
							// loc_26424

						} else {
							// loc_26543
						}
					} else {
						for (int yp = 0; yp < height1; ++yp) {
							Common::copy(srcP, srcP + width2, destP);
							destP += width2 + widthDiff2;
							srcP += width2 + widthDiff;
						}
					}
				}
			}
		}
	} else {
		error("TODO: sDrawPic");
	}
}

/**
 * Queues the given picture for display
 */
void GraphicsManager::sDisplayPic(PictureResource *pic) {
	if (pic->_flags & 8) {
		_vm->_eventsManager._intPtr.field2A = READ_LE_UINT32(pic->_imgData) >> _sImageShift;
	}

	_vm->_eventsManager._intPtr._flipWait = true;
}

void GraphicsManager::EMSMapPageHandle(int v1, int v2, int v3) {
	// TODO
}

void GraphicsManager::flipPage() {
	Common::Array<ViewPortResource *> &viewPorts = _viewPortListPtr->_entries;
	bool flipFlag = false;

	for (uint idx = 0; idx < viewPorts.size(); ++idx) {
		if (viewPorts[idx]->_flags & 0x20) {
			if ((viewPorts[idx]->_flags & 9) == 9) {
				if (_planeSelect == idx)
					sDisplayPic(viewPorts[idx]->_currentPic);
				flipFlag = true;
			}
		}

		if (flipFlag) {
			ViewPortResource &viewPort = *viewPorts[idx];

			viewPort._lastPage = viewPort._pageIndex;
			++viewPort._pageIndex;

			if (viewPort._pageIndex >= viewPort._pageCount)
				viewPort._pageIndex = 0;

			assert(viewPort._pageIndex < 2);
			viewPort._currentPic = viewPort._pages[viewPort._pageIndex];
			viewPort._flags = (viewPort._flags & 0xFFF7) | 0x40;
		}
	}
}

void GraphicsManager::restoreBack(Common::Array<Common::Rect> &rectList, int rectListCount,
		PictureResource *srcPic, PictureResource *destPic) {
	bool saveBack = _saveBack;
	_saveBack = false;

	if (rectListCount == -1) {
		sDrawPic(srcPic, destPic, Common::Point());
	} else {
		for (int i = rectListCount - 1; i >= 0; --i) {
			_clipPtr = &rectList[i];
			sDrawPic(srcPic, destPic, Common::Point());
		}
	}

	_saveBack = saveBack;
}

void GraphicsManager::clearPalette() {
	byte palette[768];
	Common::fill(&palette[0], &palette[768], 0);
	g_system->getPaletteManager()->setPalette(&palette[0], 0, 256);
}

void GraphicsManager::screenReset() {
	// TODO
}

} // End of namespace Voyeur
