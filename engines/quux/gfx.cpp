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
#include "common/system.h" // For g_system
#include "common/debug.h" // For debugC
#include "common/stream.h" // For SeekableReadStream

#include "graphics/surface.h"
#include "graphics/decoders/png.h"

#include "engines/quux/quux.h"
#include "engines/quux/gfx.h"

namespace Quux {

QuuxGFX::QuuxGFX() {
	_image = NULL;
}

QuuxGFX::~QuuxGFX() {
	// Surfaces don't automatically dispose of their
	// pixel data on destruction, so do this explicitly
	if (_image) {
		_image->free();
	}
	delete _image;
}

void QuuxGFX::init() {
	// Load an example PNG, for additional decoders see graphics/decoders/

	// Files are accessed through SearchMan, first check that it exists
	// to avoid allocating all the other objects if that is not the case.
	if (SearchMan.hasFile("example.png")) {
		// Then allocate a PNGDecoder, if you work with multiple
		// formats, you might want to use a pointer to the superclass
		// Graphics::Decoder instead.
		Graphics::PNGDecoder pngDecoder;

		// Then get a stream for the actual file, note that ScummVM-streams
		// are NOT the same as STL-streams.
		Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember("example.png");

		// Load the actual image
		pngDecoder.loadStream(*stream);

		// Then copy the image-data to a Surface that we own
		// Note: PNGDecoder MIGHT not return the expected format,
		// for instance 1 Bpp images are returned as 1 Bpp images with a palette.
		// Other decoders may also return different formats, in which case
		// you should use convertTo() instead of copyFrom()
		_image = new Graphics::Surface();
		_image->copyFrom(*pngDecoder.getSurface());

		// Finally dispose of the stream
		delete stream;
	}

	// Setup the screen to use the same screen format as the PNG-decoder.
	g_system->beginGFXTransaction();
	Graphics::PixelFormat format(4, 8, 8, 8, 8, 24, 16, 8, 0);
	g_system->initSize(640, 480, &format);
	g_system->endGFXTransaction();

	// Then set the screen-color, in this case it isn't really necessary
	// as black would be the case in most situations anyhow, this is mostly here
	// as an example, if for instance you want the clear-color to be white.
	uint32 black = g_system->getScreenFormat().ARGBToColor(0xFF, 0xFF, 0xFF, 0xFF);
	g_system->fillScreen(black);

	// Don't forget to call update-screen, much like you would need to
	// remember swapping buffers when doing double-buffered OpenGL.
	g_system->updateScreen();
}

void QuuxGFX::drawImageAt(int32 x, int32 y) {
	// As the engine doesn't have a defined set of data, the image MIGHT be missing
	if (!_image) {
		return;
	}
	int height = _image->h;
	int width = _image->w;

	// Avoid drawing outside the screen-area, by cropping.
	if (y + height >= g_system->getHeight()) {
		height = g_system->getHeight() - y;
		debugC(1, kQuuxDebugGFX, "Cropped width");
	}

	if (x + width >= g_system->getWidth()) {
		width = g_system->getWidth() - x;
		debugC(1, kQuuxDebugGFX, "Cropped height");
	}

	// Fill the screen with black again
	uint32 black = g_system->getScreenFormat().ARGBToColor(0xFF, 0xFF, 0xFF, 0xFF);
	g_system->fillScreen(black);

	// Copy the image-data to the screen (note that no actual update is done yet)
	g_system->copyRectToScreen(_image->pixels, _image->pitch, x, y, width, height);
	debugC(1, kQuuxDebugGFX, "Copied %d x %d to pos %d %d", width, height, x, y);

	// Updates only happen after THIS call.
	g_system->updateScreen();
}

void QuuxGFX::clearScreen() {
	// Fill the screen with black again
	uint32 black = g_system->getScreenFormat().ARGBToColor(0xFF, 0xFF, 0xFF, 0xFF);
	g_system->fillScreen(black);

	// Updates only happen after THIS call.
	g_system->updateScreen();
}

} // End of namespace Quux
