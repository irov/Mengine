#include "ImageDecoderHTF.h"
#include "ImageVerifyHTF.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static EPixelFormat convertFormat( uint32_t _format )
        {
            switch( _format )
            {
            case 1:
                return PF_DXT1;
            case 2:
                return PF_ETC1;
            case 3:
                return PF_PVRTC4_RGB;
            default:
                break;
            }

            return PF_UNKNOWN;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderHTF::ImageDecoderHTF()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderHTF::~ImageDecoderHTF()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderHTF::_initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderHTF::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        if( Helper::readStreamMagicHeader( stream, GET_MAGIC_NUMBER( MAGIC_HTF ), GET_MAGIC_VERSION( MAGIC_HTF ) ) == false )
        {
            LOGGER_ERROR( "invalid load magic header" );

            return false;
        }

        uint32_t width;
        stream->read( &width, sizeof( width ) );

        uint32_t height;
        stream->read( &height, sizeof( height ) );

        uint32_t format;
        stream->read( &format, sizeof( format ) );

        uint32_t mipmaps;
        stream->read( &mipmaps, sizeof( mipmaps ) );

        m_dataInfo.width = width;
        m_dataInfo.height = height;

        m_dataInfo.format = Detail::convertFormat( format );
        m_dataInfo.mipmaps = mipmaps;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderHTF::_decode( const DecoderData * _data )
    {
        void * buffer = _data->buffer;
        size_t capacity = _data->size;

        const InputStreamInterfacePtr & stream = this->getStream();

        if( Helper::readStreamArchiveInplace( stream, m_archivator, buffer, capacity, nullptr, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "invalid load" );

            return 0;
        }

        return capacity;
    }
    //////////////////////////////////////////////////////////////////////////
}
