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

#ifndef WINTERMUTE_DIRTY_RECT_CONTAINER_H
#define WINTERMUTE_DIRTY_RECT_CONTAINER_H

#include "common/rect.h"
#include "common/array.h"
#include <limits.h>

#define QUEUE_HARD_LIMIT 512
#define RETURN_HARD_LIMIT 512
#define CLEAN_HARD_LIMIT 1024
#define PIXEL_BAILOUT_LIMIT 112
#define QUEUE_BAILOUT_LIMIT 64
#define CONSISTENCY_CHECK false
#define ENABLE_BAILOUT false
#define INPUT_RECTS_HARD_LIMIT 1024

namespace Wintermute {
class DirtyRectContainer {
public:
	DirtyRectContainer();
	~DirtyRectContainer();
	/**
	 * @brief Add a dirty rect. To be called by the outside world.
	 * Doesn't necessarily result in a new dirty rect, it just informs the 
	 * DR container that an area is to be considered dirty.
	 * The DirtyRectContainer may then do what's optimal, which includes
	 * ignoring the rect altoegether (e.g. if a subset of an existing one)
	 * or disabling dirtyrects altogether.
	 */
	void addDirtyRect(const Common::Rect &rect, const Common::Rect &clipRect);
	/** 
	 * resets the DirtyRectContainer
	 */
	void reset();
	/**
	 * @brief returns on optimized list of rects where duplicates and intersections are eschewed.
	 */
	Common::Array<Common::Rect *> getOptimized();
	/**
	 * @brief returns the most naive but cheap solution - the whole viewport
	 */
	Common::Array<Common::Rect *> getFallback();
private:
	static const uint kMaxOutputRects = UINT_MAX;
	/* We have convened that we are not worried about lotsa rects
	 * anymore thanks to wjp's patch... but overflow is still a remote risk.
	 */
	static const uint kMaxInputRects = 256;
	/* Max input rects before we fall back to a single giant rect.
	 * We assume this to be an unrealistic case, if we get here something wrong has
	 * probably happened somewhere.
	 * Profiling shows that for 'reasonable' input sizes the getOptimized algorithm,
	 * which is quadratic, is irrelevant in comparison to the actual blitting, so
	 * as long as we have realistic inputs we want to save pixels, not rects.
	 */
	Common::Array<Common::Rect *> _rectArray;
	Common::Array<Common::Rect *> _cleanMe;
	// List of temporary rects created by the class to be delete()d
	// when the renderer is done with them.
	Common::Rect *_clipRect;
	/**
	 * True if DR are temporarily disabled.
	 * Only the DirtyRectContainer, single point of handling of all matters dirtyrect,
	 * may decide to do so.
	 */

	bool _tempDisableDRects;

#if CONSISTENCY_CHECK
	/**
	 * Double-checks consistency of output pixel by pixel.
	 * Returns the number of pixels that would have been drawn
	 * if overlaps were not treated, including duplicates.
	 */
	int consistencyCheck(Common::Array<Common::Rect *> &optimized);
#endif

};
} // End of namespace Wintermute

#endif
