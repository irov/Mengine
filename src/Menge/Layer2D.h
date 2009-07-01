#	pragma once

#	include "Layer.h"
#	include "Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2D
		: public Layer
	{
		FACTORABLE_DECLARE(Layer2D);

	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );
		const mt::vec2f & getParallaxFactor() const;

		void setRenderArea( const mt::vec4f& _renderArea ) override;

		bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const override;

	public:
		mt::vec2f screenToLocal( const mt::vec2f& _point );

		virtual mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const override;

		Camera* getCamera() override;

	public:
		void loader( XmlElement * _xml ) override;
		void render( unsigned int _debugMask, Camera2D* _camera ) override;

	protected:
		void _addChildren( Node * _node ) override;
		bool _activate() override;
		void _deactivate() override;

	protected:
		mt::vec2f m_factorParallax;

		Camera2D* m_camera2D;
	};
}
