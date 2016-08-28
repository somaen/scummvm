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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "titanic/game/sgt/vase.h"

namespace Titanic {

BEGIN_MESSAGE_MAP(CVase, CSGTStateRoom)
	ON_MESSAGE(TurnOn)
	ON_MESSAGE(TurnOff)
	ON_MESSAGE(MovieEndMsg)
END_MESSAGE_MAP()

void CVase::save(SimpleFile *file, int indent) {
	file->writeNumberLine(1, indent);
	CSGTStateRoom::save(file, indent);
}

void CVase::load(SimpleFile *file) {
	file->readNumber();
	CSGTStateRoom::load(file);
}

bool CVase::TurnOn(CTurnOn *msg) {
	if (CSGTStateRoom::_statics->_v3 == "Closed") {
		CSGTStateRoom::_statics->_v3 = "Open";
		setVisible(true);
		_fieldE0 = false;
		_startFrame = 1;
		_endFrame = 12;
		playMovie(1, 12, MOVIE_GAMESTATE);
	}

	return true;
}

bool CVase::TurnOff(CTurnOff *msg) {
	if (CSGTStateRoom::_statics->_v3 == "Open"
			&& CSGTStateRoom::_statics->_v1 != "RestingV"
			&& CSGTStateRoom::_statics->_v1 != "RestingUV") {
		CSGTStateRoom::_statics->_v3 = "Closed";
		_fieldE0 = true;
		_startFrame = 12;
		_endFrame = 25;
		playMovie(12, 25, MOVIE_NOTIFY_OBJECT | MOVIE_GAMESTATE);
	}

	return true;
}

bool CVase::MovieEndMsg(CMovieEndMsg *msg) {
	if (CSGTStateRoom::_statics->_v3 == "Closed")
		setVisible(false);

	return true;
}

} // End of namespace Titanic
