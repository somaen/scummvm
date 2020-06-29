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

#include "director/director.h"
#include "director/lingo/lingo.h"
#include "director/lingo/lingo-code.h"
#include "director/frame.h"
#include "director/score.h"
#include "director/sprite.h"

namespace Director {

struct EventHandlerType {
	LEvent handler;
	const char *name;
} static const eventHandlerDescs[] = {
	{ kEventPrepareMovie,		"prepareMovie" },
	{ kEventStartMovie,			"startMovie" },			//		D3
	{ kEventStepMovie,			"stepMovie" },			//		D3
	{ kEventStopMovie,			"stopMovie" },			//		D3

	{ kEventNew,				"newSprite" },
	{ kEventBeginSprite,		"beginSprite" },
	{ kEventEndSprite,			"endSprite" },

	{ kEventEnterFrame,			"enterFrame" },			//			D4
	{ kEventPrepareFrame,		"prepareFrame" },
	{ kEventIdle,				"idle" },				//		D3
	{ kEventStepFrame,			"stepFrame"},
	{ kEventExitFrame,			"exitFrame" },			//			D4

	{ kEventActivateWindow,		"activateWindow" },
	{ kEventDeactivateWindow,	"deactivateWindow" },
	{ kEventMoveWindow,			"moveWindow" },
	{ kEventResizeWindow,		"resizeWindow" },
	{ kEventOpenWindow,			"openWindow" },
	{ kEventCloseWindow,		"closeWindow" },
	{ kEventStart,				"start" },

	{ kEventKeyUp,				"keyUp" },				//			D4
	{ kEventKeyDown,			"keyDown" },			// D2 w		D4 (as when from D2)
	{ kEventMouseUp,			"mouseUp" },			// D2 w	D3
	{ kEventMouseDown,			"mouseDown" },			// D2 w	D3
	{ kEventRightMouseDown,		"rightMouseDown" },
	{ kEventRightMouseUp,		"rightMouseUp" },
	{ kEventMouseEnter,			"mouseEnter" },
	{ kEventMouseLeave,			"mouseLeave" },
	{ kEventMouseUpOutSide,		"mouseUpOutSide" },
	{ kEventMouseWithin,		"mouseWithin" },

	{ kEventTimeout,			"timeout" },			// D2 as when

	{ kEventStartUp,			"startUp" },

	{ kEventNone,				0 },
};

void Lingo::initEventHandlerTypes() {
	for (const EventHandlerType *t = &eventHandlerDescs[0]; t->handler != kEventNone; ++t) {
		_eventHandlerTypeIds[t->name] = t->handler;
		_eventHandlerTypes[t->handler] = t->name;
	}
	_eventHandlerTypes[kEventNone] = 0;
}

ScriptType Lingo::event2script(LEvent ev) {
	if (_vm->getVersion() < 4) {
		switch (ev) {
		//case kEventStartMovie: // We are precompiling it now
		//	return kMovieScript;
		case kEventEnterFrame:
			return kScoreScript;
		default:
			return kNoneScript;
		}
	}

	return kNoneScript;
}

int Lingo::getEventCount() {
	return _eventQueue.size();
}

void Lingo::setPrimaryEventHandler(LEvent event, const Common::String &code) {
	debugC(3, kDebugLingoExec, "setting primary event handler (%s)", _eventHandlerTypes[event]);
	_archives[kArchMain].primaryEventHandlers[event] = code;
	addCode(code.c_str(), kArchMain, kGlobalScript, event);
}

void Lingo::queueSpriteEvent(LEvent event, int eventId, int spriteId) {
	/* When the mouseDown or mouseUp occurs over a sprite, the message
	 * goes first to the sprite script, then to the script of the cast
	 * member, to the frame script and finally to the movie scripts.
	 *
	 * When the mouseDown or mouseUp doesn't occur over a sprite, the
	 * message goes to the frame script and then to the movie script.
	 *
	 * When more than one movie script [...]
	 * [D4 docs] */

	Score *score = _vm->getCurrentScore();
	Frame *currentFrame = score->_frames[score->getCurrentFrame()];
	assert(currentFrame != nullptr);
	Sprite *sprite = score->getSpriteById(spriteId);

	// Sprite (score) script
	if (sprite->_scriptId) {
		if (_vm->getVersion() <= 3) {
			// In D3 the event lingo is not contained in a handler
			// If sprite is immediate, its script is run on mouseDown, otherwise on mouseUp
			if ((event == kEventMouseDown && sprite->_immediate)
					|| (event == kEventMouseUp && !sprite->_immediate)) {
				_eventQueue.push(LingoEvent(kEventNone, eventId, kArchMain, kScoreScript, sprite->_scriptId, false, spriteId));
			}
		} else {
			ScriptContext *script = getScriptContext(kArchMain, kScoreScript, sprite->_scriptId);
			if (script && script->_eventHandlers.contains(event)) {
				_eventQueue.push(LingoEvent(event, eventId, kArchMain, kScoreScript, sprite->_scriptId, false, spriteId));
			}
		}
	}

	// Cast script
	int archiveIndex = kArchMain;
	ScriptContext *script = getScriptContext(archiveIndex, kCastScript, sprite->_castId);
	if (!script) {
		archiveIndex = kArchShared;
		script = getScriptContext(archiveIndex, kCastScript, sprite->_castId);
	}
	if (script && script->_eventHandlers.contains(event)) {
		_eventQueue.push(LingoEvent(event, eventId, archiveIndex, kCastScript, sprite->_castId, false, spriteId));
	}
}

void Lingo::queueFrameEvent(LEvent event, int eventId) {
	/* [in D4] the enterFrame, exitFrame, idle and timeout messages
	 * are sent to a frame script and then a movie script.	If the
	 * current frame has no frame script when the event occurs, the
	 * message goes to movie scripts.
	 * [p.81 of D4 docs]
	 */

	Score *score = _vm->getCurrentScore();

	// if (event == kEventPrepareFrame || event == kEventIdle) {
	// 	entity = score->getCurrentFrame();
	// } else {

	assert(score->_frames[score->getCurrentFrame()] != nullptr);
	int scriptId = score->_frames[score->getCurrentFrame()]->_actionId;

	if (scriptId) {
		if (event == kEventEnterFrame && _vm->getVersion() <= 3) {
			_eventQueue.push(LingoEvent(kEventNone, eventId, kArchMain, kScoreScript, scriptId, false));
		} else {
			ScriptContext *script = getScriptContext(kArchMain, kScoreScript, scriptId);
			if (script && script->_eventHandlers.contains(event)) {
				_eventQueue.push(LingoEvent(event, eventId, kArchMain, kScoreScript, scriptId, false));
			}
		}
	}
}

void Lingo::queueMovieEvent(LEvent event, int eventId) {
	/* If more than one movie script handles the same message, Lingo
	 * searches the movie scripts according to their order in the cast
	 * window [p.81 of D4 docs]
	 */

	// FIXME: shared cast movie scripts could come before main movie ones
	for (ScriptContextHash::iterator it = _archives[kArchMain].scriptContexts[kMovieScript].begin();
			it != _archives[kArchMain].scriptContexts[kMovieScript].end(); ++it) {
		if (it->_value->_eventHandlers.contains(event)) {
			_eventQueue.push(LingoEvent(event, eventId, kArchMain, kMovieScript, it->_key, false));
			return;
		}
	}
	for (ScriptContextHash::iterator it = _archives[kArchShared].scriptContexts[kMovieScript].begin();
			it != _archives[kArchShared].scriptContexts[kMovieScript].end(); ++it) {
		if (it->_value->_eventHandlers.contains(event)) {
			_eventQueue.push(LingoEvent(event, eventId, kArchShared, kMovieScript, it->_key, false));
			return;
		}
	}
}

void Lingo::registerEvent(LEvent event, int spriteId) {
	int eventId = _nextEventId++;
	if (_nextEventId < 0)
		_nextEventId = 0;

	int oldQueueSize = _eventQueue.size();

	/* When an event occurs the message [...] is first sent to a
	 * primary event handler: [... if exists it is executed] and the
	 * event is passed on to other objects unless you explicitly stop
	 * the message by including the dontPassEvent command in the script
	 * [D4 docs page 77]
	 */
	/* N.B.: No primary event handlers for events other than
	 * keyup, keydown, mouseup, mousedown, timeout
	 * [see: www.columbia.edu/itc/visualarts/r4110/s2001/handouts
	 * /03_03_Event_Hierarchy.pdf]
	 */
	switch (event) {
	case kEventMouseDown:
	case kEventMouseUp:
	case kEventKeyUp:
	case kEventKeyDown:
	case kEventTimeout:
		if (getScriptContext(kArchMain, kGlobalScript, event)) {
			_eventQueue.push(LingoEvent(kEventNone, eventId, kArchMain, kGlobalScript, event, true));
		}
		break;
	default:
		break;
	}

	/* Now queue any objects that responds to this event, in order of precedence.
	 *   (Sprite -> Cast Member -> Frame -> Movie)
	 * Once one of these objects handles the event, any event handlers queued
	 * for the same event will be ignored unless the pass command was called.
	 */
	switch (event) {
	case kEventKeyUp:
	case kEventKeyDown:
	case kEventMouseUp:
	case kEventMouseDown:
	case kEventBeginSprite:
		if (spriteId) {
			queueSpriteEvent(event, eventId, spriteId);
		}
		// fall through

	case kEventIdle:
	case kEventEnterFrame:
	case kEventExitFrame:
	case kEventNone:
		queueFrameEvent(event, eventId);
		// fall through

	case kEventStart:
	case kEventStartUp:
	case kEventStartMovie:
	case kEventStepMovie:
	case kEventStopMovie:
	case kEventTimeout:
	case kEventPrepareMovie:
		queueMovieEvent(event, eventId);
		break;

	default:
		warning("registerEvent: Unhandled event %s", _eventHandlerTypes[event]);
	}

	if (oldQueueSize == _eventQueue.size()) {
		debugC(9, kDebugEvents, "Lingo::registerEvent(%s): no event handler", _eventHandlerTypes[event]);
	}
}

void Lingo::processEvent(LEvent event, int spriteId) {
	registerEvent(event, spriteId);
	processEvents();
}

void Lingo::processEvents() {
	int lastEventId = -1;

	while (!_eventQueue.empty()) {
		LingoEvent el = _eventQueue.pop();

		if (_vm->getCurrentScore()->_stopPlay && el.event != kEventStopMovie)
			continue;

		if (lastEventId == el.eventId && !_passEvent)
			continue;

		_passEvent = el.passByDefault;
		processEvent(el.event, el.archiveIndex, el.scriptType, el.scriptId, el.channelId);
		lastEventId = el.eventId;
	}
}

void Lingo::processEvent(LEvent event, int archiveIndex, ScriptType st, int scriptId, int channelId) {
	_currentChannelId = channelId;

	if (!_eventHandlerTypes.contains(event))
		error("processEvent: Unknown event %d", event);

	ScriptContext *script = getScriptContext(archiveIndex, st, scriptId);

	if (script && script->_eventHandlers.contains(event)) {
		debugC(1, kDebugEvents, "Lingo::processEvent(%s, %d, %s, %d): executing event handler", _eventHandlerTypes[event], archiveIndex, scriptType2str(st), scriptId);
		LC::call(script->_eventHandlers[event], 0);
		execute(_pc);
	} else {
		debugC(9, kDebugEvents, "Lingo::processEvent(%s, %d, %s, %d): no handler", _eventHandlerTypes[event], archiveIndex, scriptType2str(st), scriptId);
	}
}

} // End of namespace Director
