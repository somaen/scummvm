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

#include "engines/wintermute/base/base_active_rect.h"
#include "engines/wintermute/base/base_sub_frame.h"
#include "engines/wintermute/base/gfx/base_surface.h"
#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/base_engine.h"
#include "engines/wintermute/base/base_region.h"
#include "engines/wintermute/base/gfx/base_renderer.h"
#include "engines/wintermute/platform_osystem.h"

namespace Wintermute {

//////////////////////////////////////////////////////////////////////
BaseActiveRect::BaseActiveRect(BaseGame *inGame) : BaseClass(inGame) {
	BasePlatform::setRectEmpty(&_rect);
	_owner = nullptr;
	_frame = nullptr;
	_region = nullptr;
	_zoomX = 100;
	_zoomY = 100;
	_offsetX = _offsetY = 0;
	clipRect();
}


//////////////////////////////////////////////////////////////////////
BaseActiveRect::BaseActiveRect(BaseGame *inGame, BaseObject *owner, BaseSubFrame *frame, int x, int y, int width, int height, float zoomX, float zoomY, bool precise) : BaseClass(inGame) {
	_owner = owner;
	_frame = frame;
	BasePlatform::setRect(&_rect, x, y, x + width, y + height);
	_zoomX = zoomX;
	_zoomY = zoomY;
	_precise = precise;
	_region = nullptr;
	_offsetX = _offsetY = 0;
	clipRect();
}

//////////////////////////////////////////////////////////////////////
BaseActiveRect::BaseActiveRect(BaseGame *inGame, BaseObject *owner, BaseRegion *region, int offsetX, int offsetY) : BaseClass(inGame) {
	_owner = owner;
	_region = region;
	BasePlatform::copyRect(&_rect, region->getRect());
	_rect.offsetRect(-offsetX, -offsetY);
	_zoomX = 100;
	_zoomY = 100;
	_precise = true;
	_frame = nullptr;
	clipRect();
	_offsetX = offsetX;
	_offsetY = offsetY;
}


//////////////////////////////////////////////////////////////////////
BaseActiveRect::~BaseActiveRect() {
	_owner = nullptr;
	_frame = nullptr;
	_region = nullptr;
}


//////////////////////////////////////////////////////////////////////////
void BaseActiveRect::clipRect() {
	Rect32 rc;
	bool customViewport;
	_gameRef->getCurrentViewportRect(&rc, &customViewport);
	BaseRenderer *Rend = BaseEngine::getRenderer();

	if (!customViewport) {
		rc.left -= Rend->_drawOffsetX;
		rc.right -= Rend->_drawOffsetX;
		rc.top -= Rend->_drawOffsetY;
		rc.bottom -= Rend->_drawOffsetY;
	}

	if (rc.left > _rect.left) {
		_offsetX = rc.left - _rect.left;
	}
	if (rc.top  > _rect.top) {
		_offsetY = rc.top  - _rect.top;
	}

	BasePlatform::intersectRect(&_rect, &_rect, &rc);
}

BaseObject *BaseActiveRect::getObjectAt(const Point32 &point) {
	if (BasePlatform::ptInRect(&_rect, point)) {
		if (_precise) {
			// frame
			if (_frame) {
				int xx = (int)((_frame->getRect().left + point.x - _rect.left + _offsetX) / (float)((float)_zoomX / (float)100));
				int yy = (int)((_frame->getRect().top  + point.y - _rect.top  + _offsetY) / (float)((float)_zoomY / (float)100));
				
				if (_frame->_mirrorX) {
					int width = _frame->getRect().width();
					xx = width - xx;
				}
				
				if (_frame->_mirrorY) {
					int height = _frame->getRect().height();
					yy = height - yy;
				}
				
				if (!_frame->_surface->isTransparentAt(xx, yy)) {
					return _owner;
				}
			}
			// region
			else if (getRegion()) {
				if (getRegion()->pointInRegion(point.x + _offsetX, point.y + _offsetY)) {
					return _owner;
				}
			}
		} else {
			return _owner;
		}
	}
}

} // End of namespace Wintermute
