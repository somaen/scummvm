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

#ifndef WINTERMUTE_SYSCLASS_H
#define WINTERMUTE_SYSCLASS_H

#include "persistent.h"
#include "dctypes.h"

//#include <set>
//#include <map>
#include "common/hashmap.h"
#include "common/func.h"
namespace Common {
template<typename T> struct Hash;

template<> struct Hash<void*> : public UnaryFunction<void*, uint> {
	uint operator()(void* val) const { return (uint)((size_t)val); }
};

}

namespace WinterMute {
class CSysInstance;
class CBGame;
class CBPersistMgr;
class CSysClass {
public:
	CSysClass(const AnsiString &name, PERSISTBUILD build, PERSISTLOAD load, bool persistent_class);
	~CSysClass();

	int GetNumInstances();
	bool RemoveInstance(void *instance);
	CSysInstance *AddInstance(void *instance, int id, int savedId = -1);
	bool RemoveAllInstances();

	int GetInstanceID(void *pointer);
	void *IDToPointer(int savedID);

	void SetID(int id) {
		_iD = id;
	}
	int GetID() const {
		return _iD;
	}

	int GetSavedID() const {
		return _savedID;
	}

	bool IsPersistent() const {
		return _persistent;
	}

	AnsiString GetName() const {
		return _name;
	}

	void SaveTable(CBGame *Game, CBPersistMgr *PersistMgr);
	void LoadTable(CBGame *Game, CBPersistMgr *PersistMgr);

	void SaveInstances(CBGame *Game, CBPersistMgr *PersistMgr);
	void LoadInstance(void *instance, CBPersistMgr *PersistMgr);

	void InstanceCallback(SYS_INSTANCE_CALLBACK lpCallback, void *lpData);

	void ResetSavedIDs();

	void Dump(void *stream);

private:
	int _numInst;
	bool _persistent;
	CSysClass *_next;
	int _iD;
	int _savedID;
	AnsiString _name;
	PERSISTBUILD _build;
	PERSISTLOAD _load;

	//typedef std::set<CSysInstance *> Instances;
	//Instances _instances;

	typedef Common::HashMap<void *, CSysInstance *> InstanceMap;
	InstanceMap _instanceMap;
};

} // end of namespace WinterMute

#endif
