#	pragma once

#	include "Interface/InputSystemInterface.h"

namespace Menge
{
	class InputHandler
		: public InputSystemHandler
	{
	public:		
	};

	class GlobalMouseHandler
	{
	public:
		virtual bool handleGlobalMouseButtonEvent( size_t _button, bool _isDown ) = 0;
		virtual bool handleGlobalMouseMove( int _x, int _y, int _whell ) = 0;
	};
}