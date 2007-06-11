#	pragma once

#	include "NodeImpl.h"

#	include "InputHandler.h"

namespace Menge
{
	class Avatar
		: public NodeImpl
		, public InputHandler
	{
		OBJECT_DECLARE(Avatar)

	public:
		Avatar();

	public:
		bool handlerKeyEvent( unsigned int _key, int _state ) override;
		bool handlerMouseButtonEvent( unsigned int _button, int _state ) override;
		bool handlerMouseRoll( const mt::vec3f & _pos ) override;
	};
}