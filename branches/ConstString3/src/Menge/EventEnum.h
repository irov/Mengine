#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	enum EEventName
	{
		EVENT_ANIMATION_END = 0,
		EVENT_FRAME_END,
		EVENT_FRAME_TICK,
		EVENT_EMITTER_END,
		EVENT_EMITTER_STOP,
		EVENT_MOVE_END,
		EVENT_MOVE_STOP,
		EVENT_ROTATE_END,
		EVENT_ROTATE_STOP,
		EVENT_SCALE_END,
		EVENT_SCALE_STOP,
		EVENT_COLLIDE_BEGIN,
		EVENT_COLLIDE,
		EVENT_COLLIDE_END,
		EVENT_KEY,
		EVENT_MOUSE_BUTTON,
		EVENT_MOUSE_BUTTON_END,
		EVENT_MOUSE_MOVE,
		EVENT_GLOBAL_MOUSE_BUTTON,
		EVENT_GLOBAL_MOUSE_MOVE,
		EVENT_GLOBAL_KEY,
		EVENT_LEAVE,
		EVENT_ENTER,
		EVENT_YAWTIME_END,
		EVENT_PITCHTIME_END,
		EVENT_SOUND_STOP,
		EVENT_SOUND_PAUSE,
		EVENT_COLOR_END,
		EVENT_COLOR_STOP,
		EVENT_VIDEO_END,
		EVENT_FOCUS,
		EVENT_CLOSE,
		EVENT_MOUSE_ENTER,
		EVENT_MOUSE_LEAVE,
		EVENT_ACTIVATE,
		EVENT_DEACTIVATE,
		EVENT_COMPILE,
		EVENT_RELEASE,
		EVENT_INITIALIZE,
		EVENT_FINALIZE,
		EVENT_LOADER,
		EVENT_CREATE_ACCOUNT,

		__EVENT_LAST__
	};

	extern const char * eventToString( std::size_t _event );
}
