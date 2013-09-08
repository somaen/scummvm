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

#include "common/accel_draw.h"
#include "backends/graphics/openglsdl/openglsdl-graphics.h"
#include "backends/graphics/opengl/gltexture.h"

#ifndef BACKENDS_GRAPHICS_GLACCELMAN_H
#define BACKENDS_GRAPHICS_GLACCELMAN_H

class GLAccelTexture : public Common::AccelTexture {
public:
	Graphics::Surface *_surface;
	GLTexture *_texture;
};

class GLAccelDrawManager : public Common::AccelDrawManager {
	OpenGLGraphicsManager *_system;
public:
	GLAccelDrawManager(OpenGLGraphicsManager *system) : Common::AccelDrawManager(), _system(system) {

	}
	virtual Common::AccelTexture *createTexture(const Graphics::Surface &surface) {
		GLAccelTexture *texture = new GLAccelTexture();
		texture->_surface = new Graphics::Surface();
		texture->_surface->copyFrom(surface);
		byte bpp;
		GLenum intformat;
		GLenum format;
		GLenum type;
		_system->getGLPixelFormat(surface.format, bpp, intformat, format, type);

		texture->_texture = new GLTexture(bpp, intformat, format, type);

		texture->_texture->allocBuffer(surface.w, surface.h);
		texture->_texture->updateBuffer(surface.getBasePtr(0, 0), surface.pitch, 0, 0, surface.w, surface.h);
		return texture;
	}
	virtual void drawTexture(const Common::AccelTexture *texture, const Common::Rect &sourceRect, const Common::Rect &targetRect) {
		GLAccelTexture *glText = (GLAccelTexture*)texture;
//		_system->copyRectToScreen(glText->_surface->getBasePtr(0, 0), glText->_surface->pitch, 0, 0, glText->_surface->w, glText->_surface->h);
		glText->_texture->drawTexture(targetRect.left, targetRect.top, targetRect.width(), targetRect.height(), sourceRect);
		
	}
	virtual void destroyTexture(Common::AccelTexture *texture) {
		GLAccelTexture *glText = (GLAccelTexture*)texture;
		glText->_surface->free();
		delete glText->_surface;
		delete glText->_texture;
		delete texture;
	}
	
	virtual void flipBuffer() {
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
//		_system->updateScreen();
	}
};
	


#endif
