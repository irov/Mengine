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
		OBJECT_DECLARE(HotSpotImage);

	public:
		HotSpotImage();
		~HotSpotImage();

	public:
		void loader( XmlElement *_xml ) override;

	public:
		bool pick( HotSpot * _hotspot ) override;

	private:
		String m_resourceName;
		ResourceHotspotImage* m_resouceHotspotImage;
	};
}	// namespace Menge
