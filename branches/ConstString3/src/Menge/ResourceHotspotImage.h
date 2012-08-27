/*
 *	ResourceHotspotImage.h
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ResourceReference.h"

#	include "Core/Polygon.h"
#	include "Math/vec2.h"

namespace Menge
{
	class ResourceImage;
	class AlphaChannel;

	class ResourceHotspotImage
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceHotspotImage );

	public:
		ResourceHotspotImage();

	public:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		void setImageResource( const ConstString& _resourceName );

	public:
		bool testPoint( const mt::vec2f& _point, float _minAlpha ) const;
		bool testRadius( const mt::vec2f& _point, float _radius, float _minAlpha ) const;

	public:
		const mt::vec2f& getSize() const;

	private:
		ConstString m_resourceImageName;
		ResourceImage * m_resourceImage;

		AlphaChannel * m_alphaChannel;

		size_t m_bufferOffset;

		mt::vec2f m_size;

		//size_t m_width;
		//size_t m_height;
	};
}	// namespace Menge
