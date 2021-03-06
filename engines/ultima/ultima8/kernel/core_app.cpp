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

#include "common/config-manager.h"
#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/kernel/core_app.h"
#include "ultima/ultima8/filesys/file_system.h"
#include "ultima/ultima8/conf/config_file_manager.h"

namespace Ultima {
namespace Ultima8 {

using Std::string;

CoreApp *CoreApp::_application = nullptr;

CoreApp::CoreApp(const Ultima::UltimaGameDescription *gameDesc)
		: _gameDesc(gameDesc), _isRunning(false), _gameInfo(nullptr), _fileSystem(nullptr),
		_configFileMan(nullptr){
	_application = this;
}

CoreApp::~CoreApp() {
	FORGET_OBJECT(_fileSystem);
	FORGET_OBJECT(_configFileMan);
	FORGET_OBJECT(_gameInfo);

	_application = nullptr;
}

void CoreApp::startup() {
	_gameInfo = nullptr;

	_fileSystem = new FileSystem;

	_configFileMan = new ConfigFileManager();

}

bool CoreApp::setupGame() {
	istring gamename = _gameDesc->desc.gameId;
	GameInfo *info = new GameInfo;
	bool detected = getGameInfo(gamename, info);

	// output detected game info
	debugN(MM_INFO, "%s: ", gamename.c_str());
	if (detected) {
		// add game to games map
		Std::string details = info->getPrintDetails();
		debugN(MM_INFO, "%s", details.c_str());
	} else {
		debugN(MM_INFO, "unknown, skipping");
		return false;
	}

	_gameInfo = info;

	pout << "Selected game: " << info->_name << Std::endl;
	pout << info->getPrintDetails() << Std::endl;

	// load main game data path - it needs to be empty
	//Std::string gpath = ConfMan.get("path");
	_fileSystem->AddVirtualPath("@game", "");

	return true;
}

void CoreApp::killGame() {
	_fileSystem->RemoveVirtualPath("@game");

	_configFileMan->clearRoot("bindings");
	_configFileMan->clearRoot("language");
	_configFileMan->clearRoot("weapons");
	_configFileMan->clearRoot("armour");
	_configFileMan->clearRoot("monsters");
	_configFileMan->clearRoot("game");
	_gameInfo = nullptr;
}


bool CoreApp::getGameInfo(const istring &game, GameInfo *ginfo) {
	// first try getting the information from the config file
	// if that fails, try to autodetect it

	ginfo->_name = game;
	ginfo->_type = GameInfo::GAME_UNKNOWN;
	ginfo->version = 0;
	ginfo->_language = GameInfo::GAMELANG_UNKNOWN;

	assert(game == "ultima8" || game == "remorse" || game == "regret");

	if (game == "ultima8")
		ginfo->_type = GameInfo::GAME_U8;
	else if (game == "remorse")
		ginfo->_type = GameInfo::GAME_REMORSE;
	else if (game == "regret")
		ginfo->_type = GameInfo::GAME_REGRET;

	switch (_gameDesc->desc.language) {
	case Common::EN_ANY:
		ginfo->_language = GameInfo::GAMELANG_ENGLISH;
		break;
	case Common::FR_FRA:
		ginfo->_language = GameInfo::GAMELANG_FRENCH;
		break;
	case Common::DE_DEU:
		ginfo->_language = GameInfo::GAMELANG_GERMAN;
		break;
	case Common::ES_ESP:
		ginfo->_language = GameInfo::GAMELANG_SPANISH;
		break;
	case Common::JA_JPN:
		ginfo->_language = GameInfo::GAMELANG_JAPANESE;
		break;
	default:
		error("Unknown language");
		break;
	}

	return ginfo->_type != GameInfo::GAME_UNKNOWN;
}


void CoreApp::ParseArgs(const int argc, const char *const *const argv) {
	_parameters.process(argc, argv);
}

} // End of namespace Ultima8
} // End of namespace Ultima
