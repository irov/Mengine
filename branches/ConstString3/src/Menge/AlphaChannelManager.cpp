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
			AlphaBuffer & buffer = it->second;

			this->destroyAlphaBuffer_( buffer );
		}

		m_bufferMap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	static size_t clp2( size_t x )
	{
		x = x - 1;
		x = x | (x >> 1);
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >> 16);

		return x + 1;
	}
	//////////////////////////////////////////////////////////////////////////
	AlphaChannelManager::AlphaBuffer & AlphaChannelManager::createAlphaBuffer_( const ConstString& _name, size_t _width, size_t _height )
	{
		size_t mipmap_width = clp2( _width );
		size_t mipmap_height = clp2( _height );

		AlphaBuffer ab;
		ab.width = mipmap_width;
		ab.height = mipmap_height;
		ab.ref_count = 1;
		
		while( mipmap_width || mipmap_height )
		{
			unsigned char * buff = new unsigned char[mipmap_width * mipmap_height];
						
			ab.mipmap.push_back( buff );

			mipmap_width >>= 1;
			mipmap_height >>= 1;
		}

		TBufferMap::iterator it_insert = m_bufferMap.insert( std::make_pair( _name, ab ) ).first;

		return it_insert->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannelManager::destroyAlphaBuffer_( AlphaChannelManager::AlphaBuffer & _buffer ) const
	{
		for( AlphaBuffer::TMipMapBuffers::iterator
			it = _buffer.mipmap.begin(),
			it_end = _buffer.mipmap.end();
		it != it_end;
		++it )
		{
			delete [] *it;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* AlphaChannelManager::getAlphaBuffer( const ConstString& _name, ResourceImage * _resourceImage, size_t _level, size_t & _width, size_t & _height )
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

		if( _level >= ab.mipmap.size() )
		{
			return 0;
		}
		
		return &ab.mipmap[_level][0];
	}
	//////////////////////////////////////////////////////////////////////////
	bool AlphaChannelManager::makeAlphaBuffer_( const ConstString& _name, ResourceImage * _resourceImage )
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

		AlphaBuffer & alphaBuffer = AlphaChannelManager::get()
			->createAlphaBuffer_( _name, width, height );

		ImageCodecOptions options;
		options.flags = DF_READ_ALPHA_ONLY;

		decoder->setOptions( &options );

		unsigned char * alphaMap = &alphaBuffer.mipmap[0][0];

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
			this->destroyAlphaBuffer_( it_find->second );

			m_bufferMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
