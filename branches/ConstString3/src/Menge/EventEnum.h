#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	enum EEventName
	{
		__EVENT_BEGIN__ = 0,
		EVENT_FULLSCREEN,
		EVENT_ANIMATION_END,
		EVENT_FRAME_END,
		EVENT_FRAME_TICK,
		EVENT_PARTICLE_EMITTER_END,
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
		EVENT_GLOBAL_MOUSE_BUTTON_END,
		EVENT_GLOBAL_MOUSE_MOVE,
		EVENT_MOUSE_ENTER,
		EVENT_MOUSE_LEAVE,
		EVENT_APP_MOUSE_ENTER,
		EVENT_APP_MOUSE_LEAVE,
		EVENT_GLOBAL_KEY,
		EVENT_YAWTIME_END,
		EVENT_PITCHTIME_END,
		EVENT_SOUND_STOP,
		EVENT_SOUND_PAUSE,
		EVENT_COLOR_END,
		EVENT_COLOR_STOP,
		EVENT_VIDEO_END,
		EVENT_FOCUS,
		EVENT_CLOSE,
		EVENT_ACTIVATE,
		EVENT_DEACTIVATE,
		EVENT_COMPILE,
		EVENT_RELEASE,
		EVENT_INITIALIZE,
		EVENT_PARAMS,
		EVENT_FINALIZE,
		EVENT_LOADER,
		EVENT_CREATE_ACCOUNT,
		EVENT_ON_SUB_SCENE,
		EVENT_SCHEDULE,
		EVENT_AFFECTOR,
		EVENT_PARTICLE,
		EVENT_MOVIE_FIND_INTERNAL_NODE,
		EVENT_MOVIE_FIND_INTERNAL_SPRITE,
		EVENT_MOVIE_END,
		__EVENT_LAST__
	};

	const char * eventToString( EEventName _event );
}
