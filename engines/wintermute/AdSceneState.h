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

#ifndef __WmeAdSceneState_H__
#define __WmeAdSceneState_H__

#include "persistent.h"
#include "BBase.h"
#include "coll_templ.h"

namespace WinterMute {
class CAdNodeState;
class CAdSceneState : public CBBase {
public:
	CAdNodeState *GetNodeState(char *Name, bool Saving);
	void SetFilename(char *Filename);
	DECLARE_PERSISTENT(CAdSceneState, CBBase)
	CAdSceneState(CBGame *inGame);
	virtual ~CAdSceneState();
	char *_filename;
	CBArray<CAdNodeState *, CAdNodeState *> _nodeStates;
};

} // end of namespace WinterMute

#endif
