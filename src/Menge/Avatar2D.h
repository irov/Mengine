#	pragma once

#	include "SceneNode2D.h"

#	include "InputHandler.h"

namespace Menge
{
	class Avatar2D
		: public SceneNode2D
		, public InputHandler
	{
		OBJECT_DECLARE(Avatar2D)

	public:
		Avatar2D();

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;
	};
}