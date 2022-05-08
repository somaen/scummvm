/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef TINSEL_INV_OBJECT_H
#define TINSEL_INV_OBJECT_H

#include "tinsel/dw.h"

namespace Tinsel {

/** structure of each inventory object */
struct INV_OBJECT {
	int32 id;            // inventory objects id
	SCNHANDLE hIconFilm; // inventory objects animation film
	SCNHANDLE hScript;   // inventory objects event handling script
	int32 attribute;     // inventory object's attribute
};

struct INV_OBJECT_T3 : public INV_OBJECT {
	int32 unknown;
	int32 title; // id of associated notebook title
};

class InventoryObjects {
public:
	virtual ~InventoryObjects(){};
	virtual INV_OBJECT *GetInvObject(int id) = 0;
	virtual INV_OBJECT_T3 *GetInvObjectT3(int id) = 0;
	virtual int GetObjectIndexIfExists(int id) const = 0;
	virtual INV_OBJECT *GetObjectByIndex(int index) const = 0;
	virtual int NumObjects() const = 0;
};

InventoryObjects *InstantiateInventoryObjects(INV_OBJECT *invObjects, int numObjects);

} // End of namespace Tinsel

#endif // TINSEL_INV_OBJECT_H
