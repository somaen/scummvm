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

#ifndef WINTERMUTE_BASE_REGION_H
#define WINTERMUTE_BASE_REGION_H

#include "engines/wintermute/base/base_point.h"
#include "engines/wintermute/base/base_object.h"

namespace Wintermute {

class BaseRegion : public BaseObject {
public:
	void cleanup();
	bool mimic(BaseRegion *region, float scale = 100.0f, int x = 0, int y = 0);
	bool getBoundingRect(Rect32 *rect);
	bool ptInPolygon(int32 x, int32 y) const;
	DECLARE_PERSISTENT(BaseRegion, BaseObject)

	BaseRegion(BaseGame *inGame);
	virtual ~BaseRegion();
	bool pointInRegion(int x, int y) const;
	bool createRegion();
	bool loadFile(const char *filename);
	bool loadBuffer(byte *buffer, bool complete = true);

	virtual bool saveAsText(BaseDynamicBuffer *buffer, int indent) { return saveAsText(buffer, indent, nullptr); }
	virtual bool saveAsText(BaseDynamicBuffer *buffer, int indent, const char *nameOverride);

	// scripting interface
	virtual ScValue *scGetProperty(const Common::String &name) override;
	virtual bool scSetProperty(const char *name, ScValue *value) override;
	virtual bool scCallMethod(ScScript *script, ScStack *stack, ScStack *thisStack, const char *name) override;
	virtual const char *scToString() override;

	const Rect32 *getRect() const { return &_rect; }
	bool isActive() const { return _active; }
	void setActive(bool active) { _active = active; }

protected:
	bool _active;
	int32 _editorSelectedPoint;
	BaseArray<BasePoint *> _points;

private:
	Rect32 _rect;

	float _lastMimicScale;
	int32 _lastMimicX;
	int32 _lastMimicY;
};

} // End of namespace Wintermute

#endif
