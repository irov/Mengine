/*
 *	ImageDecoderMNE.cpp
 *
 *	Created by _Berserk_ on 9.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ImageDecoderMNE.h"

#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderPNG.h"

#	include "Utils/Logger/Logger.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderMNE::ImageDecoderMNE( InputStreamInterface * _stream )
		: ImageDecoder(_stream)
		, m_jpegDecoder( NULL )
		, m_pngDecoder( NULL )
		, m_rowStride( 0 )
		, m_bufferRowStride( 0 )
		, m_png_data_seek( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderMNE::~ImageDecoderMNE()
	{
		if( m_jpegDecoder != NULL )
		{
			delete m_jpegDecoder;
			m_jpegDecoder = NULL;
		}

		if( m_pngDecoder != NULL )
		{
			delete m_pngDecoder;
			m_pngDecoder = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderMNE::initialize()
	{
		m_stream->read( &m_png_data_seek, sizeof( m_png_data_seek ) );
		m_png_data_seek += sizeof( m_png_data_seek );

		m_jpegDecoder = new ImageDecoderJPEG( m_stream );

		if( m_jpegDecoder->initialize() == false )
		{
			return false;
		}

		const ImageCodecDataInfo* jpegInfo = m_jpegDecoder->getCodecDataInfo();

		m_dataInfo.format = PF_A8R8G8B8;
		m_dataInfo.depth = 1;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.width = jpegInfo->width;
		m_dataInfo.height = jpegInfo->height;
		m_dataInfo.size = m_dataInfo.width * m_dataInfo.height * 4;
		m_rowStride = m_bufferRowStride = m_dataInfo.width * 4;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderMNE::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		bool alphaOnly = ((m_options.flags & DF_READ_ALPHA_ONLY) != 0);

		if( !alphaOnly && ((m_bufferRowStride < m_rowStride) || ((_bufferSize % m_bufferRowStride) != 0)) )
		{
			MENGE_LOG_ERROR( "ImageDecoderMNE::decode error, invalid buffer pitch or size" );
			return 0;
		}

		if( !alphaOnly && (_bufferSize < ( m_dataInfo.height * m_rowStride )) )
		{
			MENGE_LOG_ERROR( "ImageDecoderMNE::decode error - invalid buffer size. Can decode only whole image at once" );
			return 0;
		}

		if( (m_options.flags & DF_READ_ALPHA_ONLY) != 0 )
		{
			m_stream->seek( m_png_data_seek );
			
			m_pngDecoder = new ImageDecoderPNG(m_stream);

			if( m_pngDecoder->initialize() == false )
			{
				return 0;
			}

			const ImageCodecDataInfo* pngDataInfo = m_pngDecoder->getCodecDataInfo();

			// png must 1 channel 8 bit depth
			if( pngDataInfo == NULL 
				|| pngDataInfo->format != PF_A8 
				|| m_dataInfo.width != pngDataInfo->width 
				|| m_dataInfo.height != pngDataInfo->height )
			{
				MENGE_LOG_ERROR( "ImageDecoderMNE::decode error while decoding image. Can't find png data" );
				delete m_pngDecoder;
				m_pngDecoder = NULL;
				return 0;
			}

			m_pngDecoder->decode( _buffer, _bufferSize );
			delete m_pngDecoder;
			m_pngDecoder = NULL;

			return _bufferSize;
		}
		//else

		ImageCodecOptions options;
		options.flags = ( m_bufferRowStride << 16 ) | DF_COUNT_ALPHA | DF_CUSTOM_PITCH;
		m_jpegDecoder->setOptions( &options );

		

		if( m_jpegDecoder->decode( _buffer, _bufferSize ) == 0 )
		{
			return 0;
		}

		delete m_jpegDecoder;
		m_jpegDecoder = NULL;

		m_stream->seek( m_png_data_seek );

		m_pngDecoder = new ImageDecoderPNG( m_stream );
		
		if( m_pngDecoder->initialize() == false )
		{
			return false;
		}

		const ImageCodecDataInfo* pngDataInfo = m_pngDecoder->getCodecDataInfo();

		// png must 1 channel 8 bit depth
		if( pngDataInfo == NULL 
			|| pngDataInfo->format != PF_A8 
			|| m_dataInfo.width != pngDataInfo->width 
			|| m_dataInfo.height != pngDataInfo->height )
		{
			MENGE_LOG_ERROR( "ImageDecoderMNE::decode error while decoding image. Can't find png data" );
			delete m_pngDecoder;
			m_pngDecoder = NULL;
			return 0;
		}

		unsigned char* pngBuffer = new unsigned char[m_dataInfo.width];
		for( std::size_t i = 0; i < m_dataInfo.height; ++i )
		{
			m_pngDecoder->decode( pngBuffer, m_dataInfo.width );
			for( std::size_t j = 0; j < m_dataInfo.width; ++j )
			{
				_buffer[j*4+3] = pngBuffer[j];
			}
			_buffer += m_bufferRowStride;
		}
		delete[] pngBuffer;

		delete m_pngDecoder;
		m_pngDecoder = NULL;

		return m_dataInfo.height * m_rowStride;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderMNE::_invalidate()
	{
		if( ( m_options.flags & DF_CUSTOM_PITCH ) != 0 )
		{
			m_bufferRowStride = ( m_options.flags >> 16);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
