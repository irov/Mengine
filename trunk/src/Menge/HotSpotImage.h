#	pragma once

#	include "HotSpot.h"

namespace Menge
{
	class ResourceHIT;

	class HotSpotImage
		: public HotSpot
	{
	public:
		HotSpotImage();
		~HotSpotImage();

	public:
		bool testPolygon( const mt::mat4f& _transform, const Polygon& _screenPoly, const mt::mat4f& _screenTransform ) override;
		bool testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform ) override;

	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

        size_t getWidth() const;
        size_t getHeight() const;

	public:
        void setResourceHIT( ResourceHIT * _resourceHIT );
		ResourceHIT * getResourceHIT() const;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
        ResourceHIT * m_resourceHIT;

		float m_alphaTest;		
	};
}	// namespace Menge
