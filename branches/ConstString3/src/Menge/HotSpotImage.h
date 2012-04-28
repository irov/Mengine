#	pragma once

#	include "HotSpot.h"

namespace Menge
{
	class ResourceHotspotImage;

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

	public:
		ResourceHotspotImage * getResourseHotspotImage() const;

	public:
		void setImageResource( const ConstString& _resourceName );
		const ConstString & getImageResource() const;
		
	protected:
		bool _compile() override;
		void _release() override;

	private:
		ConstString m_resourceName;
		//size_t m_frame;

		float m_alphaTest;

		ResourceHotspotImage* m_resourceHotspotImage;
	};
}	// namespace Menge
