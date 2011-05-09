/*
 *	ResourceHotspotImage.cpp
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ResourceHotspotImage.h"

#	include "BinParser.h"

#	include "ResourceImplement.h"
#	include "ResourceImage.h"

#	include "Interface/ImageCodecInterface.h"
#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"

#	include "Consts.h"

#	include "CodecEngine.h"

#	include "Logger/Logger.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceHotspotImage );
	//////////////////////////////////////////////////////////////////////////
	ResourceHotspotImage::ResourceHotspotImage()
		: m_alphaMap(NULL)
		, m_resourceImageWidth(0)
		, m_resourceImageHeight(0)
		, m_offset(0.f, 0.f)
		, m_size(0.f, 0.f)
		, m_frame(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			BIN_CASE_ATTRIBUTE( Protocol::ImageResource_Name, m_resourceImageName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::_compile()
	{
		if( m_resourceImageName.empty() == true )
		{
			return false;
		}

		m_resourceImage = ResourceManager::get()
			->getResourceT<ResourceImage>( m_resourceImageName );

		if( m_resourceImage == NULL )
		{
			MENGE_LOG_ERROR("Error: ResourceHotspotImage - Image Resource '%s' not found"
				, m_resourceImageName.c_str() 
				);

			return false;
		}

		m_offset = m_resourceImage->getOffset( m_frame );
		m_size = m_resourceImage->getMaxSize( m_frame );
		const mt::vec4f& uv = m_resourceImage->getUV( m_frame );
		const mt::vec2f& size = m_resourceImage->getSize( m_frame );
		//m_resourceImageWidth = (size_t)::floorf( size.x / (uv.z - uv.x) + 0.5f );
		//m_resourceImageHeight = (size_t)::floorf( size.y / (uv.w - uv.y) + 0.5f );

		m_resourceImageWidth = size.x;
		m_resourceImageHeight = size.y;

		size_t alphaWidth = 0;
		size_t alphaHeight = 0;
		m_alphaMap = AlphaChannelManager::get()
			->getAlphaBuffer( m_resourceImageName, m_resourceImage, m_frame, alphaWidth, alphaHeight );
		
		if( m_alphaMap == NULL )
		{
			return false;
		}

		size_t offsX = (size_t)::floorf( uv.x * m_resourceImageWidth + 0.5f );
		size_t offsY = (size_t)::floorf( uv.y * m_resourceImageHeight + 0.5f );
		m_alphaMap += offsY * m_resourceImageWidth + offsX;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::_release()
	{
		AlphaChannelManager::get()
			->releaseAlphaBuffer( m_resourceImageName );

		ResourceManager::get()
			->releaseResource( m_resourceImage );

		m_alphaMap = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::setImageResource( const ConstString& _resourceName, std::size_t _frame )
	{
		m_resourceImageName = _resourceName;
		m_frame = _frame;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::testPoint( const mt::vec2f& _point, float _minAlpha /*= 0.0f */ )
	{
		float fi = ::floorf( _point.x - m_offset.x + 0.5f );
		float fj = ::floorf( _point.y - m_offset.y + 0.5f );

		if( fi < 0.f || fj < 0.f )
		{
			return false;
		}

		std::size_t i = (std::size_t)fi;
		std::size_t j = (std::size_t)fj;

		if( i >= m_resourceImageWidth || j >= m_resourceImageHeight )
		{
			return false;
		}

		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		if( m_alphaMap[j*m_resourceImageWidth + i] > minAlpha )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceHotspotImage::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
