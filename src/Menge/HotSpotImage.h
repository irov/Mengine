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
		bool testPoint( const mt::mat4f& _transform, const mt::vec2f & _point ) override;
		bool testRadius( const mt::mat4f& _transform, const mt::vec2f & _point, float _radius ) override;
		
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
        ResourceHolder<ResourceHIT> m_resourceHIT;

		float m_alphaTest;		
	};
}	// namespace Menge
