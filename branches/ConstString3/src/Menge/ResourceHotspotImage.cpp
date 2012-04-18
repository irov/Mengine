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
#	include "AlphaChannel.h"
#	include "AlphaChannelManager.h"

#	include "Consts.h"

#	include "CodecEngine.h"

#	include "LogEngine.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceHotspotImage );
	//////////////////////////////////////////////////////////////////////////
	ResourceHotspotImage::ResourceHotspotImage()
		: m_alphaChannel(NULL)
		, m_offset(0.f, 0.f)
		, m_size(0.f, 0.f)
		, m_bufferOffset(0)
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

		m_offset = m_resourceImage->getOffset();
		m_size = m_resourceImage->getMaxSize();
		const mt::vec4f& uv = m_resourceImage->getUVImage();
		const mt::vec2f& size = m_resourceImage->getSize();
		//m_resourceImageWidth = (size_t)::floorf( size.x / (uv.z - uv.x) + 0.5f );
		//m_resourceImageHeight = (size_t)::floorf( size.y / (uv.w - uv.y) + 0.5f );

		size_t imageWidth = (size_t)size.x;
		size_t imageHeight = (size_t)size.y;

		m_alphaChannel = AlphaChannelManager::get()
			->getAlphaChannel( m_resourceImageName, m_resourceImage );
		
		if( m_alphaChannel == NULL )
		{
			return false;
		}

		//size_t offsX = (size_t)::floorf( uv.x * m_resourceImageWidth + 0.5f );
		//size_t offsY = (size_t)::floorf( uv.y * m_resourceImageHeight + 0.5f );

		size_t offsetX = (size_t)uv.x * imageWidth;
		size_t offsetY = (size_t)uv.y * imageHeight;

		//m_width = (size_t)uv.z * imageWidth - offsetX;
		//m_height = (size_t)uv.w * imageHeight - offsetY;

		m_bufferOffset = offsetX + offsetY * imageWidth;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::_release()
	{
		AlphaChannelManager::get()
			->releaseAlphaBuffer( m_resourceImageName );

		if( m_resourceImage != 0 )
		{
			m_resourceImage->decrementReference();
			m_resourceImage = 0;
		}
		
		m_alphaChannel = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::setImageResource( const ConstString& _resourceName )
	{
		m_resourceImageName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::testPoint( const mt::vec2f& _point, float _minAlpha /*= 0.0f */ ) const
	{
		//float fi = ::floorf( _point.x - m_offset.x + 0.5f );
		//float fj = ::floorf( _point.y - m_offset.y + 0.5f );
		float fi = _point.x - m_offset.x;
		float fj = _point.y - m_offset.y;

		if( fi < 0.f || fj < 0.f )
		{
			return false;
		}

		size_t i = (size_t)fi;
		size_t j = (size_t)fj;

		size_t alphaWidth;
		size_t alphaHeight;

		unsigned char * alphaBuffer = m_alphaChannel->getAlphaBuffer( 0, alphaWidth, alphaHeight );

		if( i >= alphaWidth || j >= alphaHeight )
		{
			return false;
		}

		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		size_t index = j * alphaWidth + i;
				
		unsigned char alpha = alphaBuffer[index];

		if( alpha > minAlpha )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::testRadius( const mt::vec2f& _point, float _radius, float _minAlpha ) const
	{
		size_t level = (size_t)(logf( _radius ) / logf( 2.f ));

		float fi = _point.x - m_offset.x;
		float fj = _point.y - m_offset.y;
		
		if( fi < -_radius || fj < -_radius )
		{
			return false;
		}

		if( fi > m_size.x + _radius || fj > m_size.y + _radius )
		{
			return false;
		}

		if( fi < 0.f )
		{
			fi = 0.f;
		}

		if( fj < 0.f )
		{
			fj = 0.f;
		}

		if( fi > m_size.x )
		{
			fi = m_size.x;
		}

		if( fj > m_size.y )
		{
			fj = m_size.y;
		}

		size_t i = (size_t)fi;
		size_t j = (size_t)fj;

		i >>= level;
		j >>= level;

		size_t alphaWidth = 0;
		size_t alphaHeight = 0;

		unsigned char * alphaBuffer = m_alphaChannel->getAlphaBuffer( level, alphaWidth, alphaHeight );

		if( i == alphaWidth )
		{
			--i;
		}

		if( j == alphaHeight )
		{
			--j;
		}
		
		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		size_t index = i + j * alphaWidth;
				
		unsigned char alpha = alphaBuffer[index];

		if( alpha > minAlpha )
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
