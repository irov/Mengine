#include "PickDecoderHIT.h"
#include "PickVerifyHIT.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoderHIT::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        if( Helper::loadStreamMagicHeader( stream, GET_MAGIC_NUMBER( MAGIC_HIT ), GET_MAGIC_VERSION( MAGIC_HIT ) ) == false )
        {
            LOGGER_ERROR( "invalid load magic header" );

            return false;
        }

        stream->read( &m_dataInfo.width, sizeof( m_dataInfo.width ) );
        stream->read( &m_dataInfo.height, sizeof( m_dataInfo.height ) );
        stream->read( &m_dataInfo.mipmaplevel, sizeof( m_dataInfo.mipmaplevel ) );
        stream->read( &m_dataInfo.mipmapsize, sizeof( m_dataInfo.mipmapsize ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PickDecoderHIT::_decode( const DecoderData * _data )
    {
        void * buffer = _data->buffer;
        size_t capacity = _data->size;

        const InputStreamInterfacePtr & stream = this->getStream();

        if( Helper::loadStreamArchiveInplace( stream, m_archivator, buffer, capacity, nullptr, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "invalid load magic header" );

            return 0;
        }

        return capacity;
    }
    //////////////////////////////////////////////////////////////////////////
}
