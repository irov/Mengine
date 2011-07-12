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
#	include "Utils/Core/PixelFormat.h"

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

#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

namespace Menge
{
	// Special FourCC codes
	const uint32 D3DFMT_R16F			= 111;
	const uint32 D3DFMT_G16R16F			= 112;
	const uint32 D3DFMT_A16B16G16R16F	= 113;
	const uint32 D3DFMT_R32F            = 114;
	const uint32 D3DFMT_G32R32F         = 115;
	const uint32 D3DFMT_A32B32G32R32F   = 116;

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
	static PixelFormat s_convertFourCCFormat(uint32 fourcc)
	{
		// convert dxt pixel format
		switch(fourcc)
		{
		case FOURCC('D','X','T','1'):
			return PF_DXT1;
		case FOURCC('D','X','T','2'):
			return PF_DXT2;
		case FOURCC('D','X','T','3'):
			return PF_DXT3;
		case FOURCC('D','X','T','4'):
			return PF_DXT4;
		case FOURCC('D','X','T','5'):
			return PF_DXT5;
		case D3DFMT_R16F:
			return PF_FLOAT16_R;
		case D3DFMT_G16R16F:
			return PF_FLOAT16_GR;
		case D3DFMT_A16B16G16R16F:
			return PF_FLOAT16_RGBA;
		case D3DFMT_R32F:
			return PF_FLOAT32_R;
		case D3DFMT_G32R32F:
			return PF_FLOAT32_GR;
		case D3DFMT_A32B32G32R32F:
			return PF_FLOAT32_RGBA;
			// We could support 3Dc here, but only ATI cards support it, not nVidia
		}
		return PF_UNKNOWN;
	};
	//////////////////////////////////////////////////////////////////////////
	static PixelFormat s_convertPixelFormat(uint32 rgbBits, uint32 rMask, 
		uint32 gMask, uint32 bMask, uint32 aMask)
	{
		// General search through pixel formats
		for (int i = PF_UNKNOWN + 1; i < PF_COUNT; ++i)
		{
			PixelFormat pf = static_cast<PixelFormat>(i);
			if (PixelUtil::getNumElemBits(pf) == rgbBits)
			{
				uint32 testMasks[4];
				PixelUtil::getBitMasks(pf, testMasks);
				int testBits[4];
				PixelUtil::getBitDepths(pf, testBits);
				if (testMasks[0] == rMask && testMasks[1] == gMask &&
					testMasks[2] == bMask && 
					// for alpha, deal with 'X8' formats by checking bit counts
					(testMasks[3] == aMask || (aMask == 0 && testBits[3] == 0)))
				{
					return pf;
				}
			}

		}
		return PF_UNKNOWN;
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDS::ImageDecoderDDS( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem )
		: ImageDecoder(_service, _stream)
		, m_logSystem(_logSystem)
		, m_rowStride(0)
		, m_bufferRowStride(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDS::~ImageDecoderDDS()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderDDS::initialize()
	{
		DDS_HEADER header;
		uint32 magic;
		m_stream->read( &magic, sizeof( magic ) );
		if( magic != DDS_MAGIC )
		{
			LOGGER_ERROR(m_logSystem)("Invalid dds file magic number" );
			return false;
		}

		m_stream->read( &header, sizeof(DDS_HEADER) );

		//Check valid structure sizes
		if(header.dwSize != 124 && header.ddspf.dwSize != 32)
		{
			LOGGER_ERROR(m_logSystem)( "Invalid dds file header" );
			return false;
		}

		if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT && header.dwMipMapCount > 0 )
		{
			LOGGER_ERROR(m_logSystem)( "dds file has mipmaps" );
		}
		m_dataInfo.depth = 1;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.width = header.dwWidth;
		m_dataInfo.height = header.dwHeight;
		m_dataInfo.format = PF_UNKNOWN;
		m_dataInfo.flags = 0;
		
		//bool decompressDXT = false;
		// Pixel format

		if (header.ddspf.dwFlags & DDPF_FOURCC)
		{
			m_dataInfo.format = s_convertFourCCFormat(header.ddspf.dwFourCC);
		}
		else
		{
			m_dataInfo.format = s_convertPixelFormat(header.ddspf.dwRGBBitCount, 
				header.ddspf.dwRBitMask, header.ddspf.dwGBitMask, header.ddspf.dwBBitMask,
				header.ddspf.dwFlags & DDPF_ALPHAPIXELS ? header.ddspf.dwABitMask : 0);
		}
		m_dataInfo.size = PixelUtil::getMemorySize( m_dataInfo.width, m_dataInfo.height, 1, m_dataInfo.format );
		m_bufferRowStride = m_dataInfo.size;

		return true;

	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderDDS::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( (m_options.flags & DF_CUSTOM_PITCH) == false )
		{
			m_options.pitch = m_bufferRowStride;
		}

		if( (m_bufferRowStride < m_rowStride) || ((_bufferSize % m_bufferRowStride) != 0) )
		{
			LOGGER_ERROR(m_logSystem)( "ImageDecoderDDS::decode error, invalid buffer pitch or size" );

			return 0;
		}

		int read = m_stream->read( _buffer, m_dataInfo.size );

		return read * m_rowStride;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge