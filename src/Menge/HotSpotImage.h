/*
 *	HotSpotImage.h
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "HotSpot.h"

namespace Menge
{
	class ResourceHotspotImage;

	class HotSpotImage
		: public HotSpot
	{
		FACTORABLE_DECLARE(HotSpotImage);

	public:
		HotSpotImage();
		~HotSpotImage();

	public:
		void loader( XmlElement *_xml ) override;

	public:
		bool testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform ) override;

	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

	public:
		ResourceHotspotImage * getResourseHotspotImage();

	public:
		void setResourceName( const String& _resourceName );
		void setFrame( std::size_t _frame );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		String m_resourceName;
		std::size_t m_frame;

		float m_alphaTest;
		ResourceHotspotImage* m_resourceHotspotImage;
	};
}	// namespace Menge
