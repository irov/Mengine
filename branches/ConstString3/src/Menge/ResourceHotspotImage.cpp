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
		: m_imageWidth( 0 )
		, m_imageHeight( 0 )
		, m_alphaMap( NULL )
		, m_offset( 0.0f, 0.0f )
		, m_size( 0.0f, 0.0f )
		, m_frame( 0 )
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

		const ConstString& category = m_resourceImage->getCategory();
		m_alphaBufferName = m_resourceImage->getFilename( m_frame );
		m_alphaBufferCodec = m_resourceImage->getCodecType( m_frame );

		m_offset = m_resourceImage->getOffset( m_frame );
		m_size = m_resourceImage->getMaxSize( m_frame );
		const mt::vec4f& uv = m_resourceImage->getUV( m_frame );
		const mt::vec2f& size = m_resourceImage->getSize( m_frame );
		m_resourceImageWidth = (size_t)::floorf( size.x / (uv.z - uv.x) + 0.5f );
		m_resourceImageHeight = (size_t)::floorf( size.y / (uv.w - uv.y) + 0.5f );
		m_imageWidth = (size_t)::floorf( size.x + 0.5f );
		m_imageHeight = (size_t)::floorf( size.y + 0.5f );

		AlphaChannelManager* alphaMan = AlphaChannelManager::get();
		m_alphaMap = alphaMan->getAlphaBuffer( m_alphaBufferName );
		
		if( m_alphaMap == NULL )
		{
			bool isAlpha = m_resourceImage->isAlpha( m_frame );

			FileInputInterface * stream = FileEngine::get()
				->openInputFile( category, m_alphaBufferName.str() );

			ImageDecoderInterface * decoder = CodecEngine::get()
				->createDecoderT<ImageDecoderInterface>( m_alphaBufferCodec, stream );

			if( decoder == NULL )
			{
				MENGE_LOG_ERROR( "Error: ResourceHotspotImage - Can't create image decoder for file '%s'"
					, m_alphaBufferName.c_str() 
					);

				stream->close();

				return false;
			}

			const ImageCodecDataInfo* dataInfo = decoder->getCodecDataInfo();

			m_alphaMap = alphaMan->createAlphaBuffer( m_alphaBufferName, m_resourceImageWidth, m_resourceImageHeight );
			
			if( m_alphaMap == NULL )
			{
				MENGE_LOG_ERROR( "Error: (ResourceHotspotImage::_compile) failed to create alpha buffer '%s'"
					, m_alphaBufferName.c_str()
					);
				
				decoder->release();

				stream->close();

				return false;
			}

			ImageCodecOptions options;
			options.flags = DF_READ_ALPHA_ONLY;
			decoder->setOptions( &options );

			decoder->decode( m_alphaMap, m_resourceImageWidth*m_resourceImageHeight );

			decoder->release();

			stream->close();
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
			->releaseAlphaBuffer( m_alphaBufferName );

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

		if( i >= m_imageWidth || j >= m_imageHeight )
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
