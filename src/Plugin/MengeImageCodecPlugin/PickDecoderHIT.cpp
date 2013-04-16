#	include "PickDecoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Logger/Logger.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"

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
        size_t magic;

        m_stream->read( &magic, sizeof(magic) );

        if( magic != hit_magic )
        {
            return false;
        }

        size_t version;

        m_stream->read( &version, sizeof(version) );

        if( version != hit_version )
        {
            return false;
        }

        m_stream->read( &m_dataInfo.width, sizeof(m_dataInfo.width) );
        m_stream->read( &m_dataInfo.height, sizeof(m_dataInfo.height) );
        m_stream->read( &m_dataInfo.mipmaplevel, sizeof(m_dataInfo.mipmaplevel) );        
        m_stream->read( &m_dataInfo.mipmapsize, sizeof(m_dataInfo.mipmapsize) );

        m_stream->read( &m_mipmapcompresssize, sizeof(m_mipmapcompresssize) );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	unsigned int PickDecoderHIT::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{	
        static TBlobject compressBuffer;
        compressBuffer.resize( m_mipmapcompresssize );

        int read = m_stream->read( &compressBuffer[0], m_mipmapcompresssize );

        if( read == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode read 0 bite"
                );

            return 0;
        }

        size_t destLen;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress(_buffer, _bufferSize, destLen, &compressBuffer[0], m_mipmapcompresssize ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PickDecoderHIT::decode invalid uncompress"
                );

            return 0;
        }
        
		return destLen;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
