#	include "EventEnum.h"

namespace Menge
{
	static const char* s_eventToString[] = 
	{
		"onAnimationEnd",
		"onFrameEnd",
		"onFrameTick"
		"onEmitterEnd",
		"onEmitterStop",
		"onMoveEnd",
		"onMoveStop",
		"onRotateEnd",
		"onRotateStop",
		"onScaleEnd",
		"onScaleStop",
		"onUpdate",
		"onCollideBegin",
		"onCollide",
		"onCollideEnd",
		"onKey",
		"onMouseButton",
		"onMouseButtonEnd",
		"onMouseMove",
		"onGlobalMouseButton",
		"onGlobalMouseMove",
		"onGlobalKey",
		"onLeave",
		"onEnter",
		"onYawTimeEnd",
		"onPitchTimeEnd",
		"onSoundStop",
		"onSoundPause",
		"onColorEnd",
		"onColorStop",
		"onVideoEnd",
		"onFocus",
		"onClose",
		"EVENT_MOUSE_ENTER",
		"EVENT_MOUSE_LEAVE",
		"EVENT_ACTIVATE",
		"EVENT_DEACTIVATE",
		"EVENT_COMPILE",
		"EVENT_RELEASE",
		"EVENT_INITIALIZE",
		"EVENT_FINALIZE",
		"EVENT_LOADER"
	};

	const char * eventToString( std::size_t _event )
	{
		return s_eventToString[_event];
	}
}
