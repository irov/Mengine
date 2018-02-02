#	include "PickDecoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PickDecoderHIT::PickDecoderHIT()
        : m_mipmapcompresssize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PickDecoderHIT::~PickDecoderHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoderHIT::_initialize()
    {
        m_archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( m_archivator == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoderHIT::_prepareData()
    {
        if( Helper::loadStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_HIT ), GET_MAGIC_VERSION( MAGIC_HIT ) ) == false )
        {
            LOGGER_ERROR( "PickEncoderHIT::decode invalid load magic header"
            );

            return 0;
        }

        m_stream->read( &m_dataInfo.width, sizeof( m_dataInfo.width ) );
        m_stream->read( &m_dataInfo.height, sizeof( m_dataInfo.height ) );
        m_stream->read( &m_dataInfo.mipmaplevel, sizeof( m_dataInfo.mipmaplevel ) );
        m_stream->read( &m_dataInfo.mipmapsize, sizeof( m_dataInfo.mipmapsize ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PickDecoderHIT::_decode( void * _buffer, size_t _bufferSize )
    {
        if( Helper::loadStreamArchiveInplace( m_stream, m_archivator, _buffer, _bufferSize, __FILE__, __LINE__ ) == false )
        {
            LOGGER_ERROR( "PickEncoderHIT::decode invalid load magic header"
            );

            return 0;
        }

        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
}	// namespace Menge
