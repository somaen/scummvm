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

#include "tinsel/inv_objects.h"
#include "tinsel/tinsel.h"

namespace Tinsel {

template<typename T>
class InventoryObjectsImpl : public InventoryObjects {
public:
	InventoryObjectsImpl(T* invObjects, int numObjects) : _invObjects(invObjects), _numObjects(numObjects) {}
	/**
	 * Convert item ID number to pointer to item's compiled data
	 * i.e. Image data and Glitter code.
	 */
	INV_OBJECT* GetInvObject(int id) {
		int index = GetObjectIndexIfExists(id);
		if (index == -1) {
			return nullptr;
		}
		return (INV_OBJECT*)&_invObjects[index];
	}

	INV_OBJECT_T3* GetInvObjectT3(int id);

	/**
	 * Convert item ID number to index.
	 */
	int GetObjectIndexIfExists(int id) const {
		T *pObject = _invObjects;

		for (int i = 0; i < _numObjects; i++, pObject++) {
			if (pObject->id == id)
				return i;
		}
		return -1;
	}

	INV_OBJECT* GetObjectByIndex(int index) const {
		return (INV_OBJECT*)&_invObjects[index];
	}

	int NumObjects() const {
		return _numObjects;
	}
private:
	T *_invObjects = nullptr; // Inventory objects' data
	int _numObjects = 0;      // Number of inventory objects
};

// Template specializations for the Noir-query, so that we can error cleanly in non-Noir
template<>
INV_OBJECT_T3* InventoryObjectsImpl<INV_OBJECT_T3>::GetInvObjectT3(int id) {
	int index = GetObjectIndexIfExists(id);
	if (index == -1) {
		return nullptr;
	}
	return &_invObjects[index];
}

// Version for TinselVersion <= 2
template<>
INV_OBJECT_T3* InventoryObjectsImpl<INV_OBJECT>::GetInvObjectT3(int id) {
	error("Requesting INV_OBJECT_T3 from non-Noir-game");
}

InventoryObjects *InstantiateInventoryObjects(INV_OBJECT *invObjects, int numObjects) {
	if (TinselVersion == 3) {
		return new InventoryObjectsImpl<INV_OBJECT_T3>((INV_OBJECT_T3*)invObjects, numObjects);
	} else {
		return new InventoryObjectsImpl<INV_OBJECT>(invObjects, numObjects);
	}
}

} // End of namespace Tinsel