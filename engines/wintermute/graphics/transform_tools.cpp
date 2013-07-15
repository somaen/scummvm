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


#include "engines/wintermute/graphics/transform_tools.h"
#include <math.h>

namespace Wintermute {
	
	FloatPoint TransformTools::transformPoint(FloatPoint point, float rotate, Point32 zoom, bool mirrorX, bool mirrorY) {
		float rotateRad = rotate * M_PI / 180;
		FloatPoint newPoint;
		newPoint.x = (point.x * cos(rotateRad) - point.y * sin(rotateRad))*zoom.x/DEFAULT_ZOOM_X;
		newPoint.y = (point.x * sin(rotateRad) + point.y * cos(rotateRad))*zoom.y/DEFAULT_ZOOM_Y;
		if (mirrorX) {
			newPoint.x *= -1;
		}
		if (mirrorY) {
			newPoint.y *= -1;
		}
		return newPoint;
	}

	Rect32 TransformTools::newRect (Rect32 oldRect, const TransformStruct &transform, Point32 *newHotspot) {

		Point32 nw(oldRect.left, oldRect.top);
		Point32 ne(oldRect.right, oldRect.top);
		Point32 sw(oldRect.left, oldRect.bottom);
		Point32 se(oldRect.right, oldRect.bottom);
		
		FloatPoint nw1, ne1, sw1, se1;

		nw1 = transformPoint(nw - transform._hotspot, transform._angle, transform._zoom);
		ne1 = transformPoint(ne - transform._hotspot, transform._angle, transform._zoom);
		sw1 = transformPoint(sw - transform._hotspot, transform._angle, transform._zoom);
		se1 = transformPoint(se - transform._hotspot, transform._angle, transform._zoom);
		
		float top = MIN(nw1.y, MIN(ne1.y, MIN(sw1.y, se1.y)));
		float bottom = MAX(nw1.y, MAX(ne1.y, MAX(sw1.y, se1.y)));
		float left = MIN(nw1.x, MIN(ne1.x, MIN(sw1.x, se1.x)));
		float right = MAX(nw1.x, MAX(ne1.x, MAX(sw1.x, se1.x)));

		Rect32 res;
		newHotspot->y = -floor(top);
		newHotspot->x = -floor(left);

		res.top = floor(top) + transform._hotspot.y;
		res.bottom = ceil(bottom) + transform._hotspot.y; 
		res.left = floor(left) + transform._hotspot.x;
		res.right = ceil(right) + transform._hotspot.x;

		return res;
	}
} // End of namespace Wintermute
