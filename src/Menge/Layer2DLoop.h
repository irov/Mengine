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

	public:
		void update( float _timing ) override;
		void render( unsigned int _debugMask ) override;

		bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const override;
		bool testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, HotSpot * _screenspaceHotspot ) const override;
		bool testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const override;

		mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const override;
	};
}
