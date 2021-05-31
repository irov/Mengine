#include "PickEncoderHIT.h"
#include "PickVerifyHIT.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PickEncoderHIT::PickEncoderHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PickEncoderHIT::~PickEncoderHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoderHIT::_initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PickEncoderHIT::encode( const EncoderData * _data, const CodecDataInfo * _dataInfo )
    {
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const PickCodecDataInfo * );

        if( Helper::writeStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_HIT ), GET_MAGIC_VERSION( MAGIC_HIT ) ) == false )
        {
            LOGGER_ERROR( "invalid write magic header" );

            return 0;
        }

        const PickCodecDataInfo * dataInfo = static_cast<const PickCodecDataInfo *>(_dataInfo);

        m_stream->write( &dataInfo->width, sizeof( dataInfo->width ) );
        m_stream->write( &dataInfo->height, sizeof( dataInfo->height ) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof( dataInfo->mipmaplevel ) );
        m_stream->write( &dataInfo->mipmapsize, sizeof( dataInfo->mipmapsize ) );

        const void * write_buffer = _buffer;
        size_t write_size = (size_t)dataInfo->mipmapsize;

        if( Helper::writeStreamArchiveBuffer( m_stream, m_archivator, false, write_buffer, write_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "invalid write magic header" );

            return 0;
        }

        return write_size;
    }
    //////////////////////////////////////////////////////////////////////////
}
