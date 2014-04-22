#	include "ImageDecoderDDZ.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t DDS_MAGIC = 0x20534444;

	static const uint32_t DDSD_CAPS = 0x00000001;
	static const uint32_t DDSD_HEIGHT = 0x00000002;
	static const uint32_t DDSD_WIDTH = 0x00000004;
	static const uint32_t DDSD_PITCH = 0x00000008;
	static const uint32_t DDSD_PIXELFORMAT = 0x00001000;
	static const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
	static const uint32_t DDSD_LINEARSIZE = 0x00080000;
	static const uint32_t DDSD_DEPTH = 0x00800000;

	//
	// DDPIXELFORMAT flags
	//
	static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
	static const uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
	static const uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
	static const uint32_t DDPF_ALPHA = 0x00000002l;
	static const uint32_t DDPF_COMPRESSED = 0x00000080l;
	static const uint32_t DDPF_LUMINANCE = 0x00020000l;
	static const uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
	static const uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V

#pragma pack( push, 1 )
	struct DDS_PIXELFORMAT {
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};

	typedef struct {
		uint32_t           dwSize;
		uint32_t           dwFlags;
		uint32_t           dwHeight;
		uint32_t           dwWidth;
		uint32_t           dwPitchOrLinearSize;
		uint32_t           dwDepth;
		uint32_t           dwMipMapCount;
		uint32_t           dwReserved1[11];
		DDS_PIXELFORMAT	 ddspf;
		uint32_t           dwSurfaceFlags;
		uint32_t           dwCubemapFlags;
		uint32_t           dwReserved2[3];
	} DDS_HEADER;
#pragma pack(pop)

#   define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
    //////////////////////////////////////////////////////////////////////////
    static PixelFormat s_convertFourCCFormat(uint32_t fourcc)
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
        }

        return PF_UNKNOWN;
    };
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDZ::ImageDecoderDDZ()
		: m_uncompress_size(0)
		, m_compress_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDZ::~ImageDecoderDDZ()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderDDZ::_initialize()
	{		
        m_stream->read( &m_uncompress_size, sizeof(m_uncompress_size) );
        m_stream->read( &m_compress_size, sizeof(m_compress_size) );

        uint32_t magic;
        m_stream->read( &magic, sizeof(magic) );

        if( magic != DDS_MAGIC )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDS::initialize invalid dds file magic number" 
                );

            return false;
        }
        
		DDS_HEADER header;
        m_stream->read( &header, sizeof(header) );
        
		//Check valid structure sizes
		if( header.dwSize != 124 && header.ddspf.dwSize != 32)
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDS::initialize invalid dds file header" 
                );

			return false;
		}

		if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT && header.dwMipMapCount > 0 )
		{
			LOGGER_WARNING(m_serviceProvider)("ImageDecoderDDS::initialize dds file has mipmaps" 
                );                        

			return false;				 
		}

		m_dataInfo.depth = header.dwDepth;
		m_dataInfo.mipmaps = header.dwMipMapCount;
		m_dataInfo.width = header.dwWidth;
		m_dataInfo.height = header.dwHeight;
        m_dataInfo.channels = 3;
		m_dataInfo.flags = 0;
		
        //bool decompressDXT = false;
        
        if( (header.ddspf.dwFlags & DDPF_FOURCC) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDS::initialize dds file no compress" 
                );

            return false;
        }

        m_dataInfo.format = s_convertFourCCFormat( header.ddspf.dwFourCC );

        m_dataInfo.size = RENDER_SERVICE(m_serviceProvider)
            ->getMemorySize( m_dataInfo.width, m_dataInfo.height, 1, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderDDZ::decode( void * _buffer, size_t _bufferSize )
	{
        (void)_bufferSize;

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->decompress( Helper::stringizeString(m_serviceProvider, "zip"), m_stream, m_compress_size, _buffer, _bufferSize, uncompress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::decode uncompress failed"
                );

            return 0;
        }		

		return uncompress_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge