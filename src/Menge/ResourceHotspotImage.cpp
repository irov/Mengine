/*
 *	ResourceHotspotImage.cpp
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ResourceHotspotImage.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "ResourceImplement.h"
#	include "ResourceImage.h"
#	include "Codec.h"
#	include "Interface/ImageCodecInterface.h"
#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceHotspotImage );
	//////////////////////////////////////////////////////////////////////////
	ResourceHotspotImage::ResourceHotspotImage( const ResourceFactoryParam& _params )
		: ResourceReference( _params )
		, m_imageWidth( 0 )
		, m_imageHeight( 0 )
		, m_alphaMap( NULL )
		, m_offset( 0.0f, 0.0f )
		, m_size( 0.0f, 0.0f )
		, m_frame( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			XML_CASE_ATTRIBUTE_NODE( "ImageResource", "Name", m_resourceImageName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::_compile()
	{
		if( m_resourceImageName.empty() == true )
		{
			return false;
		}

		ResourceImage* resourceImage = Holder<ResourceManager>::hostage()
											->getResourceT<ResourceImage>( m_resourceImageName );
		if( resourceImage == NULL )
		{
			MENGE_LOG_ERROR("Error: ResourceHotspotImage - Image Resource \"%s\" not found"
				, m_resourceImageName.c_str() );
			return false;
		}

		String filename = resourceImage->getFactoryParams().category + resourceImage->getFilename( m_frame );
		m_offset = resourceImage->getOffset( m_frame );
		m_size = resourceImage->getMaxSize( m_frame );

		Holder<ResourceManager>::hostage()
			->releaseResource( resourceImage );

		ImageDecoderInterface* imageDecoder = Holder<DecoderManager>::hostage()
			->createDecoderT<ImageDecoderInterface>( filename, "Image" );

		if( imageDecoder == NULL )
		{
			MENGE_LOG_ERROR( "Error: ResourceHotspotImage - Can't create image decoder for file \"%s\"",
				filename.c_str() );
		}

		const ImageCodecDataInfo* dataInfo = 
			static_cast<const ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );

		m_imageWidth = dataInfo->width;
		m_imageHeight = dataInfo->height;

		m_alphaMap = new unsigned char[m_imageWidth*m_imageHeight];

		std::size_t readBufferSize = dataInfo->size / dataInfo->height;
		unsigned char* readBuffer = new unsigned char[readBufferSize];
		for( std::size_t i = 0; i < m_imageHeight; ++i )
		{
			imageDecoder->decode( readBuffer, readBufferSize );

			for( std::size_t k = 0; k < m_imageWidth; ++k )
			{
				m_alphaMap[i*m_imageWidth + k] = readBuffer[k*4+3];
			}
		}
		delete[] readBuffer;

		Holder<DecoderManager>::hostage()
			->releaseDecoder( imageDecoder );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::_release()
	{
		delete[] m_alphaMap;
		m_alphaMap = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHotspotImage::setImageResource( const String& _resourceName, std::size_t _frame )
	{
		m_resourceImageName = _resourceName;
		m_frame = _frame;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHotspotImage::testPoint( const mt::vec2f& _point, float _minAlpha /*= 0.0f */ )
	{
		std::size_t i = (std::size_t)::floorf( _point.x - m_offset.x + 0.5f );
		std::size_t j = (std::size_t)::floorf( _point.y - m_offset.y + 0.5f );

		if( i < 0 || i >= m_imageWidth
			|| j < 0 || j >= m_imageHeight )
		{
			return false;
		}

		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		if( m_alphaMap[j*m_imageWidth + i] > minAlpha )
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
