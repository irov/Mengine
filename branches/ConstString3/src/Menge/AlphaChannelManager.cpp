/*
 *	AlphaChannelManager.cpp
 *
 *	Created by _Berserk_ on 17.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "AlphaChannelManager.h"
#	include "ResourceImage.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"

#	include "Interface/ImageCodecInterface.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AlphaChannelManager::AlphaChannelManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AlphaChannelManager::~AlphaChannelManager()
	{
		for( TBufferMap::iterator 
			it = m_bufferMap.begin(), 
			it_end = m_bufferMap.end();
		it != it_end;
		++it )
		{
			delete [] it->second.buffer;
		}

		m_bufferMap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* AlphaChannelManager::createAlphaBuffer( const ConstString& _name, size_t _width, size_t _height )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		
		if( it_find != m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (AlphaChannelManager::createAlphaBuffer) buffer with name '%s' already exists"
				, _name.c_str() 
				);

			return NULL;
		}

		AlphaBuffer ab;
		ab.buffer = new unsigned char[_width*_height];
		ab.width = _width;
		ab.height = _height;
		ab.ref_count = 1;

		m_bufferMap.insert( std::make_pair( _name, ab ) );

		return ab.buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* AlphaChannelManager::getAlphaBuffer( const ConstString& _name, ResourceImage * _resourceImage, size_t & _width, size_t & _height )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		
		if( it_find == m_bufferMap.end() )
		{
			if( this->makeAlphaBuffer_( _name, _resourceImage ) == false )
			{
				return 0;
			}
	
			it_find = m_bufferMap.find( _name );
		}

		AlphaBuffer & ab = it_find->second;
		ab.ref_count += 1;

		_width = ab.width;
		_height = ab.height;
		
		return ab.buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AlphaChannelManager::makeAlphaBuffer_( const ConstString& _name, ResourceImage * _resourceImage )
	{
		const mt::vec2f & offset = _resourceImage->getOffset();
		const mt::vec2f & size = _resourceImage->getSize();

		size_t width = (size_t)size.x;
		size_t height = (size_t)size.y;

		const ConstString & alphaBufferName = _resourceImage->getFileName();
		const ConstString & alphaBufferCodec = _resourceImage->getCodecType();
		bool isAlpha = _resourceImage->isAlpha();

		const ConstString& category = _resourceImage->getCategory();

		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( category, Helper::to_str(alphaBufferName) );

		ImageDecoderInterface * decoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( alphaBufferCodec, stream );

		if( decoder == NULL )
		{
			MENGE_LOG_ERROR( "Error: ResourceHotspotImage - Can't create image decoder for file '%s'"
				, alphaBufferName.c_str() 
				);

			stream->close();

			return false;
		}

		const ImageCodecDataInfo* dataInfo = decoder->getCodecDataInfo();

		unsigned char * alphaMap = AlphaChannelManager::get()
			->createAlphaBuffer( _name, width, height );

		if( alphaMap == NULL )
		{
			MENGE_LOG_ERROR( "Error: (ResourceHotspotImage::_compile) failed to create alpha buffer '%s'"
				, alphaBufferName.c_str()
				);

			decoder->destroy();

			stream->close();

			return false;
		}

		ImageCodecOptions options;
		options.flags = DF_READ_ALPHA_ONLY;

		decoder->setOptions( &options );

		decoder->decode( alphaMap, width * height );

		decoder->destroy();

		stream->close();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannelManager::releaseAlphaBuffer( const ConstString& _name )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find == m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (AlphaChannelManager::deleteAlphaBuffer) buffer with name '%s' does not exist"
				, _name.c_str() 
				);

			return;
		}

		--it_find->second.ref_count;

		if( it_find->second.ref_count == 0 )
		{
			delete [] it_find->second.buffer;

			m_bufferMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
