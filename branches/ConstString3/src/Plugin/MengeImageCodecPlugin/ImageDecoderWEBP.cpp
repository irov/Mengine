/*
 *	ImageDecoderWEBP.cpp
 *	Copyright 2011 Menge. All rights reserved.
 *
 */

#	include "ImageDecoderWEBP.h"
#	include "Utils/Logger/Logger.h"
#	include "Interface/FileSystemInterface.h"

#	include <string.h>  // for memcpy()
#	include <math.h>    // for sqrt()
#	include "zlib.h"    // for AlphaDecode
#	include <assert.h>  // for fun )


#	define HEADER_BYTES_TO_CHECK 160
#	define ZLIB_CHUNK_SIZE  8192
#	define ALPHA_HEADER_LEN 2

//////////////////////////////////////////////////////////////////////////

// Alpha Decode.

/*
	this functions grabbed from alpha.c file from experimental WEBP brunch 
	// Copyright 2011 Google Inc.
	//
	// This code is licensed under the same terms as WebM:
	//  Software License Agreement:  http://www.webmproject.org/license/software/
	//  Additional IP Rights Grant:  http://www.webmproject.org/license/additional/
	// -----------------------------------------------------------------------------
	//
	//  RIFF container manipulation for WEBP images.
	//
	// Authors: Urvang (urvang@google.com)
	//          Vikas (vikasa@google.com)
*/
	
//////////////////////////////////////////////////////////////////////////
static int DecodeIdent(const uint8_t* data, size_t data_size,
	uint8_t* output) {
		assert((data != NULL) && (output != NULL));
		memcpy(output, data, data_size);
		return 1;
}
//////////////////////////////////////////////////////////////////////////
static int DecodeZlib(const uint8_t* data, size_t data_size,
	uint8_t* output, size_t output_size) {
		z_stream strm;
		int ret = Z_OK;

		assert((data != NULL) && (output != NULL));

		memset(&strm, 0, sizeof(strm));
		if (inflateInit(&strm) != Z_OK) {
			return 0;
		}

		strm.avail_in = data_size;
		strm.next_in = (unsigned char*)data;
		do {
			strm.avail_out = output_size;
			strm.next_out = output;
			ret = inflate(&strm, Z_NO_FLUSH);
			if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
				break;
			}
		} while (strm.avail_out == 0);

		inflateEnd(&strm);
		if (ret != Z_STREAM_END) {
			return 0;    // error
		}

		return 1;
}
//////////////////////////////////////////////////////////////////////////
int DecodeAlpha(const uint8_t* data, size_t data_size,
	int width, int height, int stride,
	uint8_t* output) {
		uint8_t* decoded_data = NULL;
		int ok = 0;
		int method;
		size_t decoded_size = height * width;

		if (data == NULL || output == NULL) {
			return 0;
		}

		if (data_size <= ALPHA_HEADER_LEN) {
			return 0;
		}

		if (width <= 0 || height <= 0 || stride < width) {
			return 0;
		}

		method = data[0];
		if (method > 2) {
			return 0;
		}

		decoded_data = (uint8_t*)malloc(decoded_size);
		if (decoded_data == NULL) {
			return 0;
		}

		data_size -= ALPHA_HEADER_LEN;
		data += ALPHA_HEADER_LEN;

		if (method == 0) {
			ok = DecodeIdent(data, data_size, decoded_data);
		} else if (method == 1) {
			ok = DecodeZlib(data, data_size, decoded_data, decoded_size);
		}
		if (ok) {
			// Construct raw_data (HeightXStride) from the alpha data (HeightXWidth).
			int h;
			for (h = 0; h < height; ++h) {
				memcpy(output + h * stride, decoded_data + h * width,
					width * sizeof(*data));
			}
		}
		free(decoded_data);

		return ok;
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::ImageDecoderWEBP( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem )
		: ImageDecoder(_service, _stream)
		, m_logSystem(_logSystem)
		, m_supportA8(false)
		, m_mux(NULL)
		, m_dataBuffer(NULL)
		, m_isValid(true)
		, m_isAlpha(false)
		, m_bufferRowStride(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::~ImageDecoderWEBP()
	{
		cleanup_();
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * ImageDecoderWEBP::getLogSystem()
	{
		return m_logSystem;
	}
	
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderWEBP::cleanup_()
	{
		if( m_mux != NULL )
		{
			WebPMuxDelete( m_mux );
		}
		if( m_dataBuffer != NULL )
		{
			delete [] m_dataBuffer;
		}
		m_isValid = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderWEBP::_invalidate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderWEBP::setOptions( CodecOptions * _info )
	{
		m_options = *static_cast<ImageCodecOptions*>(_info);

		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::initialize()
	{
		m_supportA8 = m_service->supportA8();
		int channels;
		int width;
		int height;
		int dataSize = m_stream->size();
				
		//We must write all data into buffer in initialise because WebP Mux can not determine alpha data in small chunk
		m_stream->seek( SEEK_SET );
		m_dataBuffer = new uint8_t [dataSize];
		m_stream->read(m_dataBuffer , dataSize);
		
		VP8StatusCode status = VP8_STATUS_OK;
		WebPMuxError error = WEBP_MUX_OK;
		
		unsigned char* checkHeaderBuffer = m_dataBuffer;
		// Check width and height. it stored on first 160 bytes
		if( WebPGetInfo( checkHeaderBuffer, HEADER_BYTES_TO_CHECK , (int*) &(width), (int*) &(height) ) == 0)
		{
			LOGGER_ERROR(m_logSystem)( "ImageDecoderWEBP : Bad or not WEBP file" );
			return false;
		}
		
		checkHeaderBuffer = NULL;
		
		/*
		WebPDecoderConfig config;
		if( ( WebPInitDecoderConfig(&config) == 0 ) )
		{
			printf (" Can`t init config ");
		}
		
		status = WebPGetFeatures((const uint8_t*)m_dataBuffer, dataSize, bitstream);
		if (status != VP8_STATUS_OK)
		{
			printf (" Can`t init config ");
			return 0;
		}
		*/
		
		m_mux = WebPMuxCreate((const uint8_t*)m_dataBuffer, dataSize, 0);
		if (m_mux == NULL)
		{
			printf("CAN NOT CREATE MUX");
			return false;
		}
		
		uint32_t flags;
		error = WebPMuxGetFeatures(m_mux , &flags);
		if (error != WEBP_MUX_OK)
		{
			printf("CAN NOT get Features");
			return -1;
		}
		
		if( (flags & ALPHA_FLAG) == false )
		{	
			channels = 3;
			m_isAlpha = false;
		}
		else
		{
			m_isAlpha = true;
			channels = 4;
		}
		//fill ImageCodecDataInfo strucuture
		m_dataInfo.width = width;
		m_dataInfo.height = height;
		m_dataInfo.depth = 1;
		
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.channels = channels;
		
		// Now this decoder support only RGB and ARGB
		if( m_dataInfo.channels == 4 && m_isAlpha == true )
		{
			m_dataInfo.format = PF_A8R8G8B8;
		}
		else if( m_dataInfo.channels == 3 && m_isAlpha == false )
		{
			m_dataInfo.format = PF_R8G8B8;
		}
		else
		{
			m_dataInfo.format = PF_UNKNOWN;
			m_isValid = false;
		}
		m_bufferRowStride = m_dataInfo.width * m_dataInfo.channels;	
		m_dataInfo.size = dataSize;
		//m_dataInfo.size = m_dataInfo.width * m_dataInfo.height * 4;
		m_isValid = true;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderWEBP::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{	
		
		if( m_isValid == false )
		{
			printf( "ImageDecoderWEBP Bad initialised status before decoding" );
			return 0;
		}
				
		int dataSize = m_stream->size();
				
		if( (m_options.flags & DF_CUSTOM_PITCH) == false )
		{
			m_options.pitch = m_bufferRowStride;
		}	
		
		int pitch = m_options.pitch;
				
		VP8StatusCode status = VP8_STATUS_OK;
		WebPMuxError error = WEBP_MUX_OK;
		
		//Pointers to image data
		const uint8_t* compressedAlpha = NULL;
		uint32_t compressedAlphaSize = 0;
		const uint8_t* rgbData = NULL;
		uint32_t rgbSize = 0;
		uint8_t* alphaData = NULL;
		
		error = WebPMuxGetImage( m_mux, &rgbData, &rgbSize, &compressedAlpha, &compressedAlphaSize );
		if (error != WEBP_MUX_OK)
		{
			printf("CAN NOT IMAGE");
			return 0;
		}
		
		// Delete it when decoder become stable
		bool alphaCheck;

		if( compressedAlpha!=NULL )
		{
			alphaCheck = true;
		}
		else
		{
			alphaCheck = false;
		}

		if( m_isAlpha != alphaCheck )
		{
			printf(" ImageDecoderWEBP:: differs between two methods of determining alpha data. ");
			return 0;
		}
			
		if( m_isAlpha == true )
		{
			const int width = m_dataInfo.width;
			const int height = m_dataInfo.height;
			unsigned char* bufferCursor =_buffer;
			const uint32_t alphaStride = width * sizeof( *bufferCursor );

			alphaData = new uint8_t [height * width];
			if ( !DecodeAlpha(compressedAlpha
				, compressedAlphaSize
				, width
				, height
				, width
				, alphaData))
			{
				printf("Can not decode alpha data");
				delete [] alphaData;
				return 0;
			}

			// Add Alpha to decoded RGB data.
			//Fill the buffer only with alpha channel
			if( m_options.flags & DF_READ_ALPHA_ONLY )
			{
				if( _bufferSize * 4 != m_options.pitch * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderWEBP::decode Error:  bad buffer size %i , pitch %i  height %i", _bufferSize,m_options.pitch ,m_dataInfo.height  );
					delete [] alphaData;
					return 0;
				}		

				//unsigned char* buff = new unsigned char[ m_bufferRowStride ];
				for ( std::size_t i = 0; i != m_dataInfo.height; ++i )
				{
					const uint8_t* alphaRow = alphaData + i * alphaStride;
					int alphaWidth = width / 4;
					for ( size_t j = 0; j < alphaWidth; ++j )
					{
						bufferCursor[j] = alphaRow[j];
					}
					bufferCursor += m_options.pitch / 4;
				}	
			}
			else
			{
				if( _bufferSize  != m_options.pitch * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderWEBP::decode Error:  bad buffer size %i , pitch %i  height %i", _bufferSize,m_options.pitch ,m_dataInfo.height  );
					delete [] alphaData;
					return 0;
				}		
				
				WebPDecodeBGRAInto( m_dataBuffer, dataSize,_buffer, _bufferSize, pitch );
				for ( std::size_t i = 0; i != m_dataInfo.height; ++i )
				{
					const uint8_t* alphaRow = alphaData + i * alphaStride;

					for ( size_t j = 0; j < width; ++j )
					{
						bufferCursor[4 * j + 3] = alphaRow[j];
					}

					bufferCursor += pitch;
				}
			}
			delete [] alphaData;
		}
		else
		{
			if( m_options.flags & DF_READ_ALPHA_ONLY )
			{
				printf("ImageDecoderWEBP:: DF_READ_ALPHA_ONLY not supported for images without alpha channel" );
				return 0;
			}

			if( _bufferSize  != m_options.pitch * m_dataInfo.height )
			{
				LOGGER_ERROR(m_logSystem)( "ImageDecoderWEBP::decode Error:  bad buffer size %i , pitch %i  height %i", _bufferSize,m_options.pitch ,m_dataInfo.height  );
				return 0;
			}		
			WebPDecodeBGRAInto( m_dataBuffer, dataSize,_buffer, _bufferSize, pitch );
		}
		
		return _bufferSize;
				
	}

}	// namespace Menge
