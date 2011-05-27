#	include "EventEnum.h"

namespace Menge
{
	static const char* s_eventToString[] = 
	{
		"__EVENT_BEGIN__",
		"EVENT_FULLSCREEN",
		"onAnimationEnd",
		"onFrameEnd",
		"onFrameTick",
		"EVENT_PARTICLE_EMITTER_END",
		"onMoveEnd",
		"onMoveStop",
		"onRotateEnd",
		"onRotateStop",
		"onScaleEnd",
		"onScaleStop",
		"onCollideBegin",
		"onCollide",
		"onCollideEnd",
		"onKey",
		"onMouseButton",
		"onMouseButtonEnd",
		"onMouseMove",
		"onGlobalMouseButton",
		"EVENT_GLOBAL_MOUSE_BUTTON",
		"onGlobalMouseMove",
		"EVENT_MOUSE_ENTER",
		"EVENT_MOUSE_LEAVE",
		"EVENT_APP_MOUSE_ENTER",
		"EVENT_APP_MOUSE_LEAVE",
		"onGlobalKey",
		"onYawTimeEnd",
		"onPitchTimeEnd",
		"onSoundStop",
		"onSoundPause",
		"onColorEnd",
		"onColorStop",
		"onVideoEnd",
		"onFocus",
		"onClose",
		"EVENT_PREPARATION",
		"EVENT_ACTIVATE",
		"EVENT_DEACTIVATE",
		"EVENT_COMPILE",
		"EVENT_RELEASE",
		"EVENT_INITIALIZE",
		"EVENT_PARAMS",
		"EVENT_FINALIZE",
		"EVENT_LOADER",
		"EVENT_CREATE_ACCOUNT",
		"EVENT_ON_SUB_SCENE",
		"EVENT_SCHEDULE",
		"EVENT_AFFECTOR",
		"EVENT_PARTICLE",
		"EVENT_MOVIE_FIND_INTERNAL_NODE",
		"EVENT_MOVIE_FIND_INTERNAL_SPRITE",
		"EVENT_MOVIE_END",
		"__EVENT_LAST__"
	};

	const char * eventToString( EEventName _event )
	{
		return s_eventToString[_event];
	}
}
