#	include "ImageDecoderDDZ.h"

#	include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

#   include "Config/Blobject.h"

#	include "Utils/Logger/Logger.h"

namespace Menge
{
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
        }

        return PF_UNKNOWN;
    };
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDZ::ImageDecoderDDZ()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderDDZ::~ImageDecoderDDZ()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderDDZ::_initialize( bool & _version )
	{		
		(void)_version;

        size_t file_size = m_stream->size();

        if( file_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::_initialize file zero size"
                );

            return false;
        }

        m_stream->read( &m_dds_size, sizeof(m_dds_size) );
        m_stream->read( &m_compress_size, sizeof(m_compress_size) );

        uint32 magic;
        m_stream->read( &magic, sizeof(magic) );

        if( magic != DDS_MAGIC )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDS::initialize invalid dds file magic number" 
                );

            return false;
        }
        
        m_stream->read( &m_header, sizeof(m_header) );
        
		//Check valid structure sizes
		if( m_header.dwSize != 124 && m_header.ddspf.dwSize != 32)
		{
			LOGGER_ERROR(m_serviceProvider)( "ImageDecoderDDS::initialize invalid dds file header" 
                );

			return false;
		}

		if( (m_header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT && m_header.dwMipMapCount > 0 )
		{
			LOGGER_WARNING(m_serviceProvider)( "ImageDecoderDDS::initialize dds file has mipmaps" 
                );                        
		}

		m_dataInfo.depth = m_header.dwDepth;
		m_dataInfo.mipmaps = m_header.dwMipMapCount;
		m_dataInfo.width = m_header.dwWidth;
		m_dataInfo.height = m_header.dwHeight;
        m_dataInfo.channels = 3;
		m_dataInfo.flags = 0;
		
        //bool decompressDXT = false;
        
        if( (m_header.ddspf.dwFlags & DDPF_FOURCC) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderDDS::initialize dds file no compress" 
                );

            return false;
        }

        m_dataInfo.format = s_convertFourCCFormat( m_header.ddspf.dwFourCC );

        m_dataInfo.size = RENDER_SERVICE(m_serviceProvider)
            ->getMemorySize( m_dataInfo.width, m_dataInfo.height, 1, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderDDZ::decode( void * _buffer, size_t _bufferSize )
	{
        (void)_bufferSize;

        static TBlobject compress_buf;

        compress_buf.resize( m_compress_size );

        m_stream->read( &compress_buf[0], m_compress_size );

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( _buffer, m_dds_size, uncompress_size, &compress_buf[0], m_compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::decode uncompress failed"
                );

            return 0;
        }		

		return uncompress_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge