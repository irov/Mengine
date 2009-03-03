/*
 *	ResourceHotspotImage.h
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ResourceReference.h"
#	include "Math/vec2.h"

namespace Menge
{
	class ResourceImage;

	class ResourceHotspotImage
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceHotspotImage );
	public:
		ResourceHotspotImage( const ResourceFactoryParam& _params );

		void loader( XmlElement * _xml ) override;
	protected:
		bool _compile() override;
		void _release() override;

	public:
		void setImageResource( const String& _resourceName );
		bool testPoint( const mt::vec2f& _point, float _minAlpha = 0.0f );
		const mt::vec2f& getSize() const;

	private:
		String m_resourceImageName;
		unsigned char* m_alphaMap;
		std::size_t m_imageWidth;
		std::size_t m_imageHeight;
		mt::vec2f m_offset;
		mt::vec2f m_size;
	};
}	// namespace Menge
