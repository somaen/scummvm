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

#include "common/archive.h" // For SearchMan
#include "common/stream.h" // For SeekableReadStream
#include "common/system.h" // For g_system
#include "common/textconsole.h" // For error()


#include "audio/mixer.h"
#include "audio/audiostream.h"
#include "audio/decoders/mp3.h"

#include "engines/quux/sound.h"

namespace Quux {

QuuxSound::QuuxSound() {
	_handle = NULL;
}

void QuuxSound::playIfAvailable(const Common::String &filename) {
	// Verify that the file exists.
	if (SearchMan.hasFile(filename)) {
		// Stop any already playing sound using this object's handle.
		if (_handle != NULL) {
			g_system->getMixer()->stopHandle(*_handle);
			_handle = NULL;
		}

		// Get a stream for the wanted file
		Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(filename);

		// Decode it as an MP3
		Audio::AudioStream *audioStream = Audio::makeMP3Stream(stream, DisposeAfterUse::YES);
		if (audioStream == NULL) {
			error("Error loading %s", filename.c_str());
		}
		// Play it.
		g_system->getMixer()->playStream(Audio::Mixer::kPlainSoundType, _handle, audioStream);
	}
}

} // End of namespace Quux
