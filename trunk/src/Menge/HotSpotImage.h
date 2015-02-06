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
		void setResourceHIT( ResourceHIT * _resourceHIT );
		ResourceHIT * getResourceHIT() const;

	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;

	protected:
		bool testPoint( const mt::vec2f & _point ) const override;
		bool testRadius( const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const override;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
        ResourceHolder<ResourceHIT> m_resourceHIT;

		float m_alphaTest;		
	};
}	// namespace Menge
