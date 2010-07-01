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
		ResourceHotspotImage();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		void setImageResource( const ConstString& _resourceName, std::size_t _frame );
		bool testPoint( const mt::vec2f& _point, float _minAlpha );
		const mt::vec2f& getSize() const;

	private:
		ConstString m_resourceImageName;
		ResourceImage * m_resourceImage;

		ConstString m_alphaBufferName;
		ConstString m_alphaBufferCodec;
		std::size_t m_frame;
		unsigned char* m_alphaMap;
		std::size_t m_imageWidth;
		std::size_t m_imageHeight;
		size_t m_resourceImageWidth;
		size_t m_resourceImageHeight;
		mt::vec2f m_offset;
		mt::vec2f m_size;
	};
}	// namespace Menge
