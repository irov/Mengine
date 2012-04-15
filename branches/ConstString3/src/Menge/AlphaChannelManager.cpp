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

#	include "AlphaChannel.h"

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
			AlphaChannel * buffer = it->second;
			
			buffer->destroyAlphaBuffer();

			delete buffer;			
		}

		m_bufferMap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	AlphaChannel * AlphaChannelManager::getAlphaChannel( const ConstString& _name, ResourceImage * _resourceImage )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		
		if( it_find == m_bufferMap.end() )
		{		
			AlphaChannel * alphaChannel = this->makeAlphaChannel_( _name, _resourceImage );

			if( alphaChannel == 0 )
			{
				return 0;
			}
	
			it_find = m_bufferMap.insert( std::make_pair(_name, alphaChannel) ).first;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	AlphaChannel * AlphaChannelManager::makeAlphaChannel_( const ConstString& _name, ResourceImage * _resourceImage )
	{
		const mt::vec2f & offset = _resourceImage->getOffset();
		const mt::vec2f & size = _resourceImage->getSize();

		size_t width = (size_t)size.x;
		size_t height = (size_t)size.y;

		const WString & alphaBufferName = _resourceImage->getFileName();
		const ConstString & alphaBufferCodec = _resourceImage->getCodecType();
		bool isAlpha = _resourceImage->isAlpha();

		const ConstString& category = _resourceImage->getCategory();

		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( category, alphaBufferName );

		ImageDecoderInterface * decoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( alphaBufferCodec, stream );

		if( decoder == NULL )
		{
			MENGE_LOG_ERROR( "AlphaChannelManager::makeAlphaBuffer_ %s create image decoder for file '%S'"
				, _name.c_str()
				, alphaBufferName.c_str() 
				);

			stream->close();

			return false;
		}
		
		ImageCodecOptions options;
		options.flags = DF_READ_ALPHA_ONLY;

		decoder->setOptions( &options );

		AlphaChannel * alphaChannel = new AlphaChannel(width, height);
		unsigned char * alphaMap = alphaChannel->createAlphaBuffer();

		decoder->decode( alphaMap, width * height );

		alphaChannel->makeMipMapLevel();

		decoder->destroy();

		stream->close();

		return alphaChannel;
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannelManager::releaseAlphaBuffer( const ConstString& _name )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find == m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "AlphaChannelManager::releaseAlphaBuffer buffer with name '%s' does not exist"
				, _name.c_str() 
				);

			return;
		}

		AlphaChannel * alphaChannel = it_find->second;

		if( alphaChannel->releaseAlphaBuffer() == true )
		{
			m_bufferMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
