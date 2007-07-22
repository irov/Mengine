#	pragma once

#	include "Layer.h"
#	include "SceneNode3D.h"

#	include "Viewport.h"

#	include <list>

namespace Menge
{
	class Camera3D;

	class Layer3D
		: public Layer
		, public SceneNode3D
	{
		OBJECT_DECLARE(Layer3D);

	public:
		Layer3D();

	public:
		void loader( TiXmlElement * _xml) override;
		void renderLayer() override;

		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;

	};
}