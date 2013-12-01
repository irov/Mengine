#	include "ImageDecoderPVRTZ.h"

#	include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	enum PixelType
	{
		PVRTC2RGB = 0,
		PVRTC2RGBA,
		PVRTC4RGB,
		PVRTC4RGBA,
		PVRTCII2,
		PVRTCII4,
		ETC1,
		DXT1,
		DXT2,
		DXT3,
		DXT4,
		DXT5,
		BC1,
		BC2,
		BC3,
		BC4,
		BC5,
		BC6,
		BC7,
		UYVY,
		YUY2,
		BW1bpp,
		R9G9B9E5,
		RGBG8888,
		GRGB8888,
		ETC2RGB,
		ETC2RGBA,
		ETC2RGBA1,
		EACR11U,
		EACR11S,
		EACRG11U,
		EACRG11S,
	};
	//////////////////////////////////////////////////////////////////////////
	const unsigned int PVRTEX_MIPMAP = (1<<8); // has mip map levels
	const unsigned int PVRTEX_TWIDDLE = (1<<9); // is twiddled
	const unsigned int PVRTEX_BUMPMAP = (1<<10); // has normals encoded for a bump map
	const unsigned int PVRTEX_TILING = (1<<11); // is bordered for tiled pvr
	const unsigned int PVRTEX_CUBEMAP = (1<<12); // is a cubemap/skybox
	const unsigned int PVRTEX_FALSEMIPCOL  = (1<<13); //
	const unsigned int PVRTEX_VOLUME = (1<<14);
	const unsigned int PVRTEX_PIXELTYPE = 0xff; // pixel type is always in the last 16bits of the flags
	const unsigned int PVRTEX_IDENTIFIER  = 0x21525650;  // the pvr identifier is the characters 'P','V','R'

	const unsigned int PVRTEX_V1_HEADER_SIZE = 44; // old header size was 44 for identification purposes

	const unsigned int PVRTC2_MIN_TEXWIDTH = 16;
	const unsigned int PVRTC2_MIN_TEXHEIGHT = 8;
	const unsigned int PVRTC4_MIN_TEXWIDTH = 8;
	const unsigned int PVRTC4_MIN_TEXHEIGHT = 8;
	const unsigned int ETC_MIN_TEXWIDTH = 4;
	const unsigned int ETC_MIN_TEXHEIGHT = 4;
	//////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
	typedef struct
	{
		uint32_t    version;            
		uint32_t    flags;          
		uint64_t    pixelFormat;        
		uint32_t    colourSpace;        
		uint32_t    channelType;        
		uint32_t    height;         
		uint32_t    width;          
		uint32_t    depth;          
		uint32_t    numSurfaces;        
		uint32_t    numFaces;       
		uint32_t    numMipmaps;     
		uint32_t    metaDataSize; 	
	} PVRTC_HEADER;
#pragma pack(pop)
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPVRTZ::ImageDecoderPVRTZ()
		: m_uncompress_size(0)
		, m_compress_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPVRTZ::~ImageDecoderPVRTZ()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPVRTZ::_initialize( bool & _version )
	{
		(void)_version;

		m_stream->read( &m_uncompress_size, sizeof(m_uncompress_size) );
		m_stream->read( &m_compress_size, sizeof(m_compress_size) );

		PVRTC_HEADER header;
		m_stream->read( &header, sizeof(header) );

		if( header.numFaces == 1 && // supported only 1 face
			header.numSurfaces == 1 &&  // supported only 1 surface
			header.numMipmaps < 2 ) // mipmapping is disabled, so is not supported.
		{
			return false;
		}
		
		int compressedImageSize = ( (std::max)(header.width, PVRTC4_MIN_TEXWIDTH) * (std::max)(header.height, PVRTC4_MIN_TEXHEIGHT) * 4 + 7) / 8;
		m_dataInfo.size = compressedImageSize; //m_pvrtc_ptr.dwTextureDataSize;
		m_dataInfo.channels = 3;
		m_dataInfo.depth = 1;

		switch (header.pixelFormat)
		{
		case PVRTC4RGB:
			{
				m_dataInfo.format = Menge::PF_PVRTC4_RGB;
			}break;
		case PVRTC4RGBA:
			{
				m_dataInfo.format = Menge::PF_PVRTC4_RGBA;
			}break;
		case ETC1:
			{
				m_dataInfo.format = Menge::PF_ETC1;
			}break;
		default:
			{
				LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPVRTZ::initialize not supported format"				
					);

				return false;
			}
		}

		m_dataInfo.width = header.width;
		m_dataInfo.height = header.height;
		m_dataInfo.mipmaps = header.numMipmaps;
		m_dataInfo.flags = 0;

		m_stream->seek( header.metaDataSize );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderPVRTZ::decode( void * _buffer, size_t _bufferSize )
	{
		(void)_bufferSize;

		static TBlobject s_compress_cache;

		s_compress_cache.resize( m_compress_size );
		TBlobject::value_type * compress_buffer = &s_compress_cache[0];

		m_stream->read( compress_buffer, m_compress_size );

		size_t uncompress_size;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( _buffer, m_uncompress_size, uncompress_size, compress_buffer, m_compress_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::decode uncompress failed"
				);

			return 0;
		}		

		return uncompress_size;
	}
}