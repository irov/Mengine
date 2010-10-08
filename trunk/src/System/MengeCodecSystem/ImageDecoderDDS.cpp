/*
 *	ImageDecoderJPEG.cpp
 *
 *	Created by _Berserk_ on 4.10.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#	include "ImageDecoderDDS.h"

#	include "Interface/FileSystemInterface.h"
#	include "Utils/Logger/Logger.h"

#define DDS_MAGIC			0x20534444

#define DDSD_CAPS			0x00000001
#define DDSD_HEIGHT			0x00000002
#define DDSD_WIDTH			0x00000004
#define DDSD_PITCH			0x00000008
#define DDSD_PIXELFORMAT 	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE		0x00080000
#define DDSD_DEPTH			0x00800000 

//
// DDPIXELFORMAT flags
//
#define DDPF_ALPHAPIXELS        0x00000001l
#define DDPF_FOURCC             0x00000004l        // Compressed formats
#define DDPF_RGB                0x00000040l        // Uncompressed formats
#define DDPF_ALPHA              0x00000002l
#define DDPF_COMPRESSED         0x00000080l
#define DDPF_LUMINANCE          0x00020000l
#define DDPF_BUMPLUMINANCE      0x00040000l        // L,U,V
#define DDPF_BUMPDUDV           0x00080000l        // U,V

namespace Menge
{

#pragma pack( push, 1 )
	struct DDS_PIXELFORMAT {
		uint32 dwSize;
		uint32 dwFlags;
		uint32 dwFourCC;
		uint32 dwRGBBitCount;
		uint32 dwRBitMask;
		uint32 dwGBitMask;
		uint32 dwBBitMask;
		uint32 dwABitMask;
	};

	typedef struct {
		uint32           dwSize;
		uint32           dwFlags;
		uint32           dwHeight;
		uint32           dwWidth;
		uint32           dwPitchOrLinearSize;
		uint32           dwDepth;
		uint32           dwMipMapCount;
		uint32           dwReserved1[11];
		DDS_PIXELFORMAT	 ddspf;
		uint32           dwSurfaceFlags;
		uint32           dwCubemapFlags;
		uint32           dwReserved2[3];
	} DDS_HEADER;
#pragma pack( pop )
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDS::ImageDecoderDDS()
		: m_rowStride( 0 )
		, m_bufferRowStride( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDS::~ImageDecoderDDS()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderDDS::_initialize()
	{
		if( m_stream != NULL )
		{
			m_valid = readHeader_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderDDS::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( m_valid == false )
		{
			return 0;
		}

		if( (m_bufferRowStride < m_rowStride) || ((_bufferSize % m_bufferRowStride) != 0) )
		{
			MENGE_LOG_ERROR( "ImageDecoderDDS::decode error, invalid buffer pitch or size" );
			return 0;
		}

		int read = 0;
		//MENGE_LOG_INFO( "ImageDecoderJPEG::decode 1" );

		/*while( (m_jpegObject->output_scanline < m_jpegObject->output_height) && (_bufferSize >= m_bufferRowStride) ) 
		{
			jpeg_read_scanlines( m_jpegObject, &_buffer, 1 );
			read++;
			
			if( (m_options & DF_COUNT_ALPHA) != 0 )	// place data as there is alpha
			{
				// place a little magic here =)
				std::size_t bufferDataWidth = m_dataInfo.width * 4;
				for( std::size_t i = 0; i < m_dataInfo.width; i++ )
				{
					std::copy( _buffer + 3 * ( m_dataInfo.width - i - 1 ), _buffer + 3 * ( m_dataInfo.width - i ), _buffer + bufferDataWidth - 4 - i*4 );
					_buffer[bufferDataWidth-i*4-1] = 255; // alpha
					
				}
			}

			// Assume put_scanline_someplace wants a pointer and sample count.
			_buffer += m_bufferRowStride;
			_bufferSize -= m_bufferRowStride;
		}*/
		//MENGE_LOG_INFO( "ImageDecoderJPEG::decode 2" );

		return read * m_rowStride;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderDDS::setOptions( unsigned int _options )
	{
		ImageDecoder::setOptions( _options );

		if( ( m_options & DF_CUSTOM_PITCH ) != 0 )
		{
			m_bufferRowStride = ( m_options >> 16);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderDDS::readHeader_()
	{
		DDS_HEADER header;
		uint32 magic;
		m_stream->read( &magic, sizeof( magic ) );
		if( magic != DDS_MAGIC )
		{
			MENGE_LOG_ERROR( "Invalid dds file magic number" );
			return false;
		}

		m_stream->read( &header, sizeof(DDS_HEADER) );

		//Check valid structure sizes
		if(header.dwSize != 124 && header.ddspf.dwSize != 32)
		{
			MENGE_LOG_ERROR( "Invalid dds file header" );
			return false;
		}

		if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT && header.dwMipMapCount > 0 )
		{
			MENGE_LOG_WARNING( "dds file has mipmaps" );
		}
		m_dataInfo.depth = 1;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.width = header.dwWidth;
		m_dataInfo.height = header.dwHeight;
		m_dataInfo.format = PF_UNKNOWN;
		m_dataInfo.flags = 0;
		
		bool decompressDXT = false;
		// Pixel format

		/*if (header.ddspf.dwFlags & DDPF_FOURCC)
		{
			m_dataInfo.format = convertFourCCFormat(header.ddspf.dwFourCC);
		}
		else
		{
			sourceFormat = convertPixelFormat(header.ddspf.dwRGBBitCount, 
				header.ddspf.dwRBitMask, header.ddspf.dwGBitMask, header.ddspf.dwBBitMask,
				header.ddspf.dwFlags & DDPF_ALPHAPIXELS ? header.ddspf.dwABitMask : 0);
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
