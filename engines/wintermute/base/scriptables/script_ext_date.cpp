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

#include "engines/wintermute/base/scriptables/script_stack.h"
#include "engines/wintermute/base/scriptables/script_value.h"
#include "engines/wintermute/base/scriptables/script_ext_date.h"

namespace Wintermute {

IMPLEMENT_PERSISTENT(SXDate, false)

BaseScriptable *makeSXDate(BaseGame *inGame, ScStack *stack) {
	return new SXDate(inGame, stack);
}

//////////////////////////////////////////////////////////////////////////
SXDate::SXDate(BaseGame *inGame, ScStack *stack) : BaseScriptable(inGame) {
	stack->correctParams(6);

	memset(&_tm, 0, sizeof(_tm));

	ScValue *valYear = stack->pop();
	_tm.tm_year = valYear->getInt() - 1900;
	_tm.tm_mon = stack->pop()->getInt() - 1;
	_tm.tm_mday = stack->pop()->getInt();
	_tm.tm_hour = stack->pop()->getInt();
	_tm.tm_min = stack->pop()->getInt();
	_tm.tm_sec = stack->pop()->getInt();

	if (valYear->isNULL()) {
		g_system->getTimeAndDate(_tm);
	}
}


//////////////////////////////////////////////////////////////////////////
SXDate::~SXDate() {

}

//////////////////////////////////////////////////////////////////////////
Common::String SXDate::scToString() {
	// TODO: Make this more stringy, and less ISO 8601-like
	_strRep.format("%04d-%02d-%02d - %02d:%02d:%02d", _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
	return _strRep.c_str();
	//return asctime(&_tm);
}


//////////////////////////////////////////////////////////////////////////
bool SXDate::scCallMethod(ScScript *script, ScStack *stack, ScStack *thisStack, const Common::String &name) {
	//////////////////////////////////////////////////////////////////////////
	// GetYear
	//////////////////////////////////////////////////////////////////////////
	if (name == "GetYear") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_year + 1900);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetMonth
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetMonth") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_mon + 1);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetDate
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetDate") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_mday);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetHours
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetHours") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_hour);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetMinutes
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetMinutes") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_min);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetSeconds
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetSeconds") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_sec);
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GetWeekday
	//////////////////////////////////////////////////////////////////////////
	else if (name == "GetWeekday") {
		stack->correctParams(0);
		stack->pushInt(_tm.tm_wday);
		return STATUS_OK;
	}


	//////////////////////////////////////////////////////////////////////////
	// SetYear
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetYear") {
		stack->correctParams(1);
		_tm.tm_year = stack->pop()->getInt() - 1900;
		stack->pushNULL();
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetMonth
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetMonth") {
		stack->correctParams(1);
		_tm.tm_mon = stack->pop()->getInt() - 1;
		stack->pushNULL();
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetDate
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetDate") {
		stack->correctParams(1);
		_tm.tm_mday = stack->pop()->getInt();
		stack->pushNULL();
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetHours
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetHours") {
		stack->correctParams(1);
		_tm.tm_hour = stack->pop()->getInt();
		stack->pushNULL();
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetMinutes
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetMinutes") {
		stack->correctParams(1);
		_tm.tm_min = stack->pop()->getInt();
		stack->pushNULL();
		return STATUS_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetSeconds
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetSeconds") {
		stack->correctParams(1);
		_tm.tm_sec = stack->pop()->getInt();
		stack->pushNULL();
		return STATUS_OK;
	}


	//////////////////////////////////////////////////////////////////////////
	// SetCurrentTime
	//////////////////////////////////////////////////////////////////////////
	else if (name == "SetCurrentTime") {
		stack->correctParams(0);
		g_system->getTimeAndDate(_tm);
		stack->pushNULL();
		return STATUS_OK;
	} else {
		return STATUS_FAILED;
	}
}


//////////////////////////////////////////////////////////////////////////
ScValue *SXDate::scGetProperty(const Common::String &name) {
	_scValue->setNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if (name == "Type") {
		_scValue->setString("date");
		return _scValue;
	} else {
		return _scValue;
	}
}


//////////////////////////////////////////////////////////////////////////
bool SXDate::scSetProperty(const Common::String &name, ScValue *value) {
	/*
	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	if (name == "Name")==0) {
	    setName(value->getString());
	    return STATUS_OK;
	}

	else*/ return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool SXDate::persist(BasePersistenceManager *persistMgr) {

	BaseScriptable::persist(persistMgr);
	int32 year = _tm.tm_year;
	int32 mon = _tm.tm_mon;
	int32 mday = _tm.tm_mday;
	int32 hour = _tm.tm_hour;
	int32 min = _tm.tm_min;
	int32 sec = _tm.tm_sec;
	persistMgr->transfer(TMEMBER(year));
	persistMgr->transfer(TMEMBER(mon));
	persistMgr->transfer(TMEMBER(mday));
	persistMgr->transfer(TMEMBER(hour));
	persistMgr->transfer(TMEMBER(min));
	persistMgr->transfer(TMEMBER(sec));
	if (persistMgr->checkVersion(1, 2, 1)) {
		int32 wday = _tm.tm_wday;
		persistMgr->transfer(TMEMBER(wday));
		_tm.tm_wday = wday;
	}
	_tm.tm_year = year;
	_tm.tm_mon = mon;
	_tm.tm_mday = mday;
	_tm.tm_hour = hour;
	_tm.tm_min = min;
	_tm.tm_sec = sec;
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
int SXDate::scCompare(BaseScriptable *Value) {
	TimeDate time1 = _tm;
	TimeDate time2 = ((SXDate *)Value)->_tm;

	if (time1.tm_year < time2.tm_year) {
		return -1;
	} else if (time1.tm_year == time2.tm_year) {
		if (time1.tm_mon < time2.tm_mon) {
			return -1;
		} else if (time1.tm_mon == time2.tm_mon) {
			if (time1.tm_mday < time2.tm_mday) {
				return -1;
			} else if (time1.tm_mday == time2.tm_mday) {
				if (time1.tm_hour < time2.tm_hour) {
					return -1;
				} else if (time1.tm_hour == time2.tm_hour) {
					if (time1.tm_min < time2.tm_min) {
						return -1;
					} else if (time1.tm_min == time2.tm_min) {
						if (time1.tm_sec < time2.tm_sec) {
							return -1;
						} else if (time1.tm_sec == time2.tm_sec) {
							return 0; // Equal
						} else {
							return 1; // Sec
						}
					} else {
						return 1; // Minute
					}
				} else {
					return 1; // Hour
				}
			} else {
				return 1; // Day
			}
		} else {
			return 1; // Month
		}
	} else {
		return 1; // Year
	}
}

} // End of namespace Wintermute
