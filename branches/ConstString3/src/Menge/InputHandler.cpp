#	include "InputHandler.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool InputHandler::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputHandler::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputHandler::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		return false;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool InputHandler::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputHandler::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		return false;
	}
}
