#	pragma once

#	include "Layer2D.h"
#	include "Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2DLoop
		: public Layer2D
	{
		OBJECT_DECLARE(Layer2DLoop);

	public:
		Layer2DLoop();

		void update( float _timing ) override;
		void render( const Viewport & _viewport, bool _enableDebug ) override;

		bool isLooped() { return true; }
	};
}