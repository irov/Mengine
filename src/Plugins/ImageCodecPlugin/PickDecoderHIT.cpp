#include "PickDecoderHIT.h"
#include "PickVerifyHIT.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
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
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoderHIT::_prepareData()
    {
        if( Helper::loadStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_HIT ), GET_MAGIC_VERSION( MAGIC_HIT ) ) == false )
        {
            LOGGER_ERROR( "invalid load magic header"
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
        if( Helper::loadStreamArchiveInplace( m_stream, m_archivator, _buffer, _bufferSize, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "invalid load magic header"
            );

            return 0;
        }

        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
}
