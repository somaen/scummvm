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
#include "common/savefile.h"
#include "common/system.h"
#include "common/installshield_cab.h"

#include "engines/advancedDetector.h"
#include "engines/obsolete.h"

#include "agos/intern.h"
#include "agos/agos.h"
#include "agos/detection/detection.h"
#include "agos/obsolete.h"

class AgosMetaEngineConnect : public AdvancedMetaEngineConnect {
public:
    const char *getName() const override {
		return "agos";
	}

    bool hasFeature(MetaEngineFeature f) const override;

	Common::Error createInstance(OSystem *syst, Engine **engine) const override {
		Engines::upgradeTargetIfNecessary(obsoleteGameIDsTable);
		return AdvancedMetaEngineConnect::createInstance(syst, engine);
	}
	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;

	SaveStateList listSaves(const char *target) const override;
	int getMaximumSaveSlot() const override;
};

bool AgosMetaEngineConnect::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSimpleSavesNames);
}

bool AGOS::AGOSEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsReturnToLauncher);
}

bool AgosMetaEngineConnect::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const AGOS::AGOSGameDescription *gd = (const AGOS::AGOSGameDescription *)desc;
	bool res = true;

	switch (gd->gameType) {
	case AGOS::GType_PN:
		*engine = new AGOS::AGOSEngine_PN(syst, gd);
		break;
	case AGOS::GType_ELVIRA1:
		*engine = new AGOS::AGOSEngine_Elvira1(syst, gd);
		break;
	case AGOS::GType_ELVIRA2:
		*engine = new AGOS::AGOSEngine_Elvira2(syst, gd);
		break;
	case AGOS::GType_WW:
		*engine = new AGOS::AGOSEngine_Waxworks(syst, gd);
		break;
	case AGOS::GType_SIMON1:
		*engine = new AGOS::AGOSEngine_Simon1(syst, gd);
		break;
	case AGOS::GType_SIMON2:
		*engine = new AGOS::AGOSEngine_Simon2(syst, gd);
		break;
#ifdef ENABLE_AGOS2
	case AGOS::GType_FF:
		if (gd->features & AGOS::GF_DEMO)
			*engine = new AGOS::AGOSEngine_FeebleDemo(syst, gd);
		else
			*engine = new AGOS::AGOSEngine_Feeble(syst, gd);
		break;
	case AGOS::GType_PP:
		if (gd->gameId == AGOS::GID_DIMP)
			*engine = new AGOS::AGOSEngine_DIMP(syst, gd);
		else
			*engine = new AGOS::AGOSEngine_PuzzlePack(syst, gd);
		break;
#endif
	default:
		res = false;
		error("AGOS engine: unknown gameType");
	}

	return res;
}

SaveStateList AgosMetaEngineConnect::listSaves(const char *target) const {
	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Common::StringArray filenames;
	Common::String saveDesc;
	Common::String pattern = target;
	pattern += ".###";

	filenames = saveFileMan->listSavefiles(pattern);

	SaveStateList saveList;
	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); ++file) {
		// Obtain the last 3 digits of the filename, since they correspond to the save slot
		int slotNum = atoi(file->c_str() + file->size() - 3);

		if (slotNum >= 0 && slotNum <= 999) {
			Common::InSaveFile *in = saveFileMan->openForLoading(*file);
			if (in) {
				saveDesc = file->c_str();
				saveList.push_back(SaveStateDescriptor(slotNum, saveDesc));
				delete in;
			}
		}
	}

	// Sort saves based on slot number.
	Common::sort(saveList.begin(), saveList.end(), SaveStateDescriptorSlotComparator());
	return saveList;
}

int AgosMetaEngineConnect::getMaximumSaveSlot() const { return 999; }

#if PLUGIN_ENABLED_DYNAMIC(AGOS)
	REGISTER_PLUGIN_DYNAMIC(AGOS, PLUGIN_TYPE_ENGINE, AgosMetaEngineConnect);
#else
	REGISTER_PLUGIN_STATIC(AGOS, PLUGIN_TYPE_ENGINE, AgosMetaEngineConnect);
#endif

namespace AGOS {

int AGOSEngine::getGameId() const {
	return _gameDescription->gameId;
}

int AGOSEngine::getGameType() const {
	return _gameDescription->gameType;
}

uint32 AGOSEngine::getFeatures() const {
	return _gameDescription->features;
}

const char *AGOSEngine::getExtra() const {
	return _gameDescription->desc.extra;
}

Common::Language AGOSEngine::getLanguage() const {
	return _gameDescription->desc.language;
}

Common::Platform AGOSEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

const char *AGOSEngine::getFileName(int type) const {
	// Required if the InstallShield cab is been used
	if (getGameType() == GType_PP) {
		if (type == GAME_BASEFILE)
			return gss->base_filename;
	}

	// Required if the InstallShield cab is been used
	if (getGameType() == GType_FF && getPlatform() == Common::kPlatformWindows) {
		if (type == GAME_BASEFILE)
			return gss->base_filename;
		if (type == GAME_RESTFILE)
			return gss->restore_filename;
		if (type == GAME_TBLFILE)
			return gss->tbl_filename;
	}

	for (int i = 0; _gameDescription->desc.filesDescriptions[i].fileType; i++) {
		if (_gameDescription->desc.filesDescriptions[i].fileType == type)
			return _gameDescription->desc.filesDescriptions[i].fileName;
	}
	return NULL;
}

#ifdef ENABLE_AGOS2
void AGOSEngine::loadArchives() {
	const ADGameFileDescription *ag;

	if (getFeatures() & GF_PACKED) {
		for (ag = _gameDescription->desc.filesDescriptions; ag->fileName; ag++) {
			if (!SearchMan.hasArchive(ag->fileName)) {
				Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(ag->fileName);

				if (stream)
					SearchMan.add(ag->fileName, Common::makeInstallShieldArchive(stream, DisposeAfterUse::YES), ag->fileType);
			}
		}
	}
}
#endif

} // End of namespace AGOS
