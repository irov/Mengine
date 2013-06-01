#	include "ImageDecoderDDZ.h"

#	include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Utils/Logger/Logger.h"
#	include "Utils/Core/PixelFormat.h"

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
	bool ImageDecoderDDZ::_initialize()
	{		
        size_t file_size = m_stream->size();

        if( file_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::_initialize file zero size"
                );

            return false;
        }

        size_t dds_size;
        m_stream->read( &dds_size, sizeof(dds_size) );

        size_t compress_size;
        m_stream->read( &compress_size, sizeof(compress_size) );

        static TBlobject compress_buf;

        compress_buf.resize( compress_size );

        m_stream->read( &compress_buf[0], compress_size );

        m_buffer.resize( dds_size );
        
        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( &m_buffer[0], dds_size, uncompress_size, &compress_buf[0], compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDZ::_initialize uncompress failed"
                );

            return false;
        }
        
        m_dds_size = uncompress_size;
        
        TBlobject::value_type * buffer = &m_buffer[0];

		uint32 magic;
        memcpy( &magic, buffer, sizeof(magic) );

		if( magic != DDS_MAGIC )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderDDS::initialize invalid dds file magic number" 
                );

			return false;
		}
                
        memcpy( &m_header, buffer + sizeof(magic), sizeof(DDS_HEADER) );

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
		m_dataInfo.num_mipmaps = m_header.dwMipMapCount;
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

        m_dataInfo.size = PixelUtil::getMemorySize( m_dataInfo.width, m_dataInfo.height, 1, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderDDZ::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
        size_t dds_size = m_dds_size - sizeof(uint32) - sizeof(DDS_HEADER);

        TBlobject::value_type * buffer = &m_buffer[0];

        buffer += sizeof(uint32);
        buffer += sizeof(DDS_HEADER);

        memcpy( _buffer, buffer, dds_size );
		//size_t read = m_stream->read( _buffer, dds_size );

		return dds_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge