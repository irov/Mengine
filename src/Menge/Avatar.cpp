#	include "Avatar.h"

#	include "ObjectImplement.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Avatar)
	//////////////////////////////////////////////////////////////////////////
	Avatar::Avatar()
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar::handleMouseMove( float _x, float _y, float _whell )
	{
		return false;
	}
}