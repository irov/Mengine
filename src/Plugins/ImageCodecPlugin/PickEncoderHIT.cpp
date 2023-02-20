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
    void PickEncoderHIT::_finalize()
    {
        m_archivator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PickEncoderHIT::encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _encoderData );
        MENGINE_ASSERTION_TYPE( _encoderData, const PickEncoderData * );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const PickCodecDataInfo * );

        const PickCodecDataInfo * dataInfo = static_cast<const PickCodecDataInfo *>(_dataInfo);

        if( Helper::writeStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_HIT ), GET_MAGIC_VERSION( MAGIC_HIT ) ) == false )
        {
            LOGGER_ERROR( "invalid write magic header" );

            return 0;
        }

        m_stream->write( &dataInfo->width, sizeof( dataInfo->width ) );
        m_stream->write( &dataInfo->height, sizeof( dataInfo->height ) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof( dataInfo->mipmaplevel ) );
        m_stream->write( &dataInfo->mipmapsize, sizeof( dataInfo->mipmapsize ) );

        const PickEncoderData * encoderData = static_cast<const PickEncoderData *>(_encoderData);

        const void * write_buffer = encoderData->buffer;
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
