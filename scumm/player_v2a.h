/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001-2003 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#ifndef PLAYER_V2A_H
#define PLAYER_V2A_H

#include "common/scummsys.h"
#include "common/system.h"
#include "scumm/music.h"
#include "scumm/player_mod.h"

#define	V2A_MAXSLOTS 8

class Scumm;
class SoundMixer;

class V2A_Sound;

class Player_V2A : public MusicEngine {
public:
	Player_V2A(Scumm *scumm);
	virtual ~Player_V2A();

	virtual void setMasterVolume(int vol);
	virtual void startSound(int nr);
	virtual void stopSound(int nr);
	virtual void stopAllSounds();
	virtual int  getMusicTimer() const;
	virtual int  getSoundStatus(int nr) const;

private:
	OSystem *_system;
	Scumm *_scumm;
	Player_MOD *_mod;

	struct soundSlot
	{
		int id;
		V2A_Sound *sound;
	} _slot[V2A_MAXSLOTS];
	int getSoundSlot (int id = 0) const;

	static void update_proc(void *param);
	void updateSound();
};

#endif
