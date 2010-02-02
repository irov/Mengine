#	include "EventEnum.h"

namespace Menge
{
	static const char* s_eventToString[] = 
	{
		"onAnimationEnd",
		"onFrameEnd",
		"onEmitterEnd",
		"onEmitterStop",
		"onMoveEnd",
		"onMoveStop",
		"onRotateEnd",
		"onRotateStop",
		"onScaleEnd",
		"onScaleStop",
		"onUpdate",
		"onCollide",
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
		"EVENT_MOUSE_LEAVE"
	};

	const char * eventToString( std::size_t _event )
	{
		return s_eventToString[_event];
	}
}