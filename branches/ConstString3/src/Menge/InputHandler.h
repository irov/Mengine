#	pragma once

#	include "Interface/InputSystemInterface.h"

namespace Menge
{
	class InputHandler
		: public InputSystemHandler
	{
	public:		
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		bool handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;
	};

	class GlobalMouseHandler
	{
	public:
		virtual bool handleGlobalMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) = 0;
		virtual bool handleGlobalMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) = 0;
		virtual bool handleGlobalMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) = 0;
		virtual bool handleGlobalMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) = 0;
	};

	class GlobalKeyHandler
	{
	public:
		virtual bool handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) = 0;
	};
}
