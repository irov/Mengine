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
		bool testPoint( const mt::vec2f & _point ) const override;
		bool testRadius( const mt::vec2f & _point, float _radius ) const override;
		
	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;

	public:
        void setResourceHIT( ResourceHIT * _resourceHIT );
		ResourceHIT * getResourceHIT() const;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
        ResourceHolder<ResourceHIT> m_resourceHIT;

		float m_alphaTest;		
	};
}	// namespace Menge
