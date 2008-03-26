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
		virtual bool handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown ) = 0;
		virtual bool handleGlobalMouseMove( float _x, float _y, int _whell ) = 0;
	};

	class GlobalKeyHandler
	{
	public:
		virtual bool handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) = 0;
	};
}