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

#include "ags/lib/allegro.h"
#include "ags/plugins/dll.h"
#include "ags/plugins/ags_creditz/ags_creditz.h"
#include "ags/ags.h"
#include "ags/detection.h"
#include "common/str.h"

namespace AGS3 {
namespace Plugins {

void *dlopen(const char *filename) {
	Common::String fname(filename);

	// Check for if the game specifies a specific plugin version for this game
	int version = 0;
	for (const ::AGS::PluginVersion *v = ::AGS::g_vm->getNeededPlugins();
	        v->_plugin; ++v) {
		if (Common::String::format("lib%s.so", v->_plugin).equalsIgnoreCase(filename)) {
			version = v->_version;
			break;
		}
	}

	if (fname.equalsIgnoreCase("libagsCreditz.so")) {
		if (version == 20)
			return new AGSCreditz::AGSCreditz20();
		else
			return new AGSCreditz::AGSCreditz11();
	}

	return nullptr;
}

int dlclose(void *lib) {
	DLL *dll = static_cast<DLL *>(lib);
	delete dll;
	return 0;
}

void *dlsym(void *lib, const char *method) {
	DLL *dll = static_cast<DLL *>(lib);
	return (*dll)[method];
}

const char *dlerror() {
	return nullptr;
}

/*------------------------------------------------------------------*/

DLL::DLL() {
	DLL_METHOD(AGS_PluginV2);
	DLL_METHOD(AGS_EditorStartup);
	DLL_METHOD(AGS_EditorShutdown);
	DLL_METHOD(AGS_EditorProperties);
	DLL_METHOD(AGS_EditorSaveGame);
	DLL_METHOD(AGS_EditorLoadGame);
	DLL_METHOD(AGS_EngineStartup);
	DLL_METHOD(AGS_EngineShutdown);
	DLL_METHOD(AGS_EngineOnEvent);
	DLL_METHOD(AGS_EngineDebugHook);
	DLL_METHOD(AGS_EngineInitGfx);
}

int DLL::AGS_EditorStartup(IAGSEditor *) {
	return 0;
}

void DLL::AGS_EditorShutdown() {
}

void DLL::AGS_EditorProperties(HWND) {
}

int DLL::AGS_EditorSaveGame(char *, int) {
	return 0;
}

void DLL::AGS_EditorLoadGame(char *, int) {
}

void DLL::AGS_EngineStartup(IAGSEngine *) {
}

void DLL::AGS_EngineShutdown() {
}

NumberPtr DLL::AGS_EngineOnEvent(int, NumberPtr) {
	return 0;
}

int DLL::AGS_EngineDebugHook(const char *, int, int) {
	return 0;
}

void DLL::AGS_EngineInitGfx(const char *driverID, void *data) {
	assert(!strcmp(driverID, "Software"));
}

} // namespace Plugins
} // namespace AGS3
