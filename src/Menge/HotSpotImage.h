#	pragma once

#	include "HotSpot.h"

#   include "ResourceHIT.h"

namespace Menge
{
	class HotSpotImage
		: public HotSpot
	{
	public:
		HotSpotImage();
		~HotSpotImage();

	public:
		void setResourceHIT( const ResourceHITPtr & _resourceHIT );
		const ResourceHITPtr & getResourceHIT() const;

	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;

	protected:
		bool testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Viewport & _gameViewport, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Viewport & _gameViewport, const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Viewport & _gameViewport, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _debugRender( const RenderObjectState * _state, unsigned int _debugMask ) override;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
        ResourceHolder<ResourceHIT> m_resourceHIT;

		float m_alphaTest;		
	};
}	// namespace Menge
