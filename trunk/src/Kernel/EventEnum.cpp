#	include "Kernel/EventEnum.h"

namespace Menge
{
	static const char* s_eventToString[] = 
	{
		"__EVENT_BEGIN__",
		"EVENT_DESTROY",
		"EVENT_FULLSCREEN",
        "EVENT_FIXED_CONTENT_RESOLUTION",
        "EVENT_RENDER_VIEWPORT",
		"onAnimationEnd",
		"onFrameEnd",
		"onFrameTick",
		"EVENT_PARTICLE_EMITTER_END",
		"EVENT_PARTICLE_EMITTER_RESTART",
		"onMoveEnd",
		"onMoveStop",
		"onRotateEnd",
		"onRotateStop",
		"onScaleEnd",
		"onScaleStop",
		"onCollideBegin",
		"onCollide",
		"onCollideEnd",
		"EVENT_KEY",
		"EVENT_KEY2",
		"EVENT_MOUSE_BUTTON",
		"EVENT_MOUSE_BUTTON2",
		"EVENT_MOUSE_BUTTON_BEGIN",
		"EVENT_MOUSE_BUTTON_END",
		"EVENT_MOUSE_MOVE",
		"EVENT_GLOBAL_MOUSE_BUTTON",
		"EVENT_GLOBAL_MOUSE_BUTTON2",
		"EVENT_GLOBAL_MOUSE_BUTTON_BEGIN",
		"EVENT_GLOBAL_MOUSE_BUTTON_END",
		"EVENT_GLOBAL_MOUSE_MOVE",
		"EVENT_MOUSE_ENTER",
		"EVENT_MOUSE_LEAVE",
		"EVENT_APP_MOUSE_ENTER",
		"EVENT_APP_MOUSE_LEAVE",
		"EVENT_GLOBAL_KEY",
		"EVENT_GLOBAL_KEY2",
		"onYawTimeEnd",
		"onPitchTimeEnd",
		"EVENT_SOUND_END",
		"onSoundPause",
		"onColorEnd",
		"onColorStop",
		"onVideoEnd",
		"onFocus",
        "EVENT_USER",
		"onClose",
		"EVENT_PREPARATION",
		"EVENT_ACTIVATE",
        "EVENT_PREPARATION_DEACTIVATE",
		"EVENT_DEACTIVATE",
		"EVENT_COMPILE",
		"EVENT_RELEASE",
		"EVENT_INITIALIZE",
        "EVENT_INITIALIZE_RENDER_RESOURCES",
		"EVENT_PARAMS",
        "EVENT_ACCOUNT_FINALIZE",
		"EVENT_FINALIZE",
		"EVENT_CREATE",
		"EVENT_CREATE_ACCOUNT",
        "EVENT_DELETE_ACCOUNT",
		"EVENT_SELECT_ACCOUNT",
        "EVENT_UNSELECT_ACCOUNT",
		"EVENT_ON_SUB_SCENE",
		"EVENT_SCHEDULE",
		"EVENT_TIMING",
		"EVENT_AFFECTOR",
		"EVENT_AFFECTOR_END",
		"EVENT_PARTICLE",
		"EVENT_MOVIE_GET_INTERNAL",
		"EVENT_MOVIE_HIDE_INTERNAL",
		"EVENT_MOVIE_ACTIVATE_INTERNAL",
		"EVENT_MOVIE_DEACTIVATE_INTERNAL",
		"EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION",
		"EVENT_MOVIE_APPLY_INTERNAL_OPACITY",
		"EVENT_MOVIE_END",
		"EVENT_MOVIE_LOOP",
		"EVENT_ON_TIMING_FACTOR",
		"EVENT_CURSOR_MODE",
		"__EVENT_LAST__"
	};

	const char * eventToString( EEventName _event )
	{
		return s_eventToString[_event];
	}
}
