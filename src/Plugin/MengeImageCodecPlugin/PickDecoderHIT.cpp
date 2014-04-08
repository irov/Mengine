#	include "PickDecoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickDecoderHIT::PickDecoderHIT()
        : m_mipmapcompresssize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PickDecoderHIT::~PickDecoderHIT()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoderHIT::_initialize()
    {
        uint32_t magic;
        m_stream->read( &magic, sizeof(magic) );

        if( magic != hit_magic )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode invalid magic"
                );

            return false;
        }
		       
		uint32_t version;
        m_stream->read( &version, sizeof(version) );

        if( version != hit_version )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode invalid hit version %d need %d"
                , version
                , hit_version
                );
			
            return false;
        }

        m_stream->read( &m_dataInfo.width, sizeof(m_dataInfo.width) );
        m_stream->read( &m_dataInfo.height, sizeof(m_dataInfo.height) );
        m_stream->read( &m_dataInfo.mipmaplevel, sizeof(m_dataInfo.mipmaplevel) );
        m_stream->read( &m_dataInfo.mipmapsize, sizeof(m_dataInfo.mipmapsize) );	

        m_stream->read( &m_mipmapcompresssize, sizeof(m_mipmapcompresssize) );

        if( m_mipmapcompresssize == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode invalid m_mipmapcompresssize is 0 (hit file incorupt)"
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t PickDecoderHIT::decode( void * _buffer, size_t _bufferSize )
	{	        
		CacheMemoryBuffer compress_buffer(m_serviceProvider, m_mipmapcompresssize, "PickDecoderHIT");
		void * compress_memory = compress_buffer.getMemory();

        size_t read = m_stream->read( compress_memory, m_mipmapcompresssize );

        if( read == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode read 0 bite"
                );

            return 0;
        }

        size_t decompressSize;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->decompress( _buffer, _bufferSize, compress_memory, m_mipmapcompresssize, decompressSize ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode invalid uncompress"
                );

            return 0;
        }
        
		return decompressSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
