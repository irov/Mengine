#	pragma once

#	include "Math/vec3.h"

namespace Menge
{
	class InputHandler
	{
	public:
		virtual bool handlerKeyEvent( unsigned int _key, int _state ) = 0;
		virtual bool handlerMouseButtonEvent( unsigned int _button, int _state ) = 0;
		virtual bool handlerMouseRoll( const mt::vec3f & _pos ) = 0;
	};
}