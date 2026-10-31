#include "WebAudioSoundBuffer.h"

#include "WebAudioBridge.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundBuffer::WebAudioSoundBuffer()
        : m_bufferId( 0 )
        , m_duration( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundBuffer::~WebAudioSoundBuffer()
    {
        mengineAudioBufferRelease( m_bufferId );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WebAudioSoundBuffer::getBufferId() const
    {
        return m_bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    float WebAudioSoundBuffer::getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundBuffer::load( const SoundDecoderInterfacePtr & _decoder, bool _streamable )
    {
        const SoundCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

        if( dataInfo->channels == 0 )
        {
            LOGGER_ERROR( "invalid sound channels: %u"
                , dataInfo->channels
            );

            return false;
        }

        uint32_t bufferId;
        if( _streamable == true )
        {
            bufferId = this->createStream_( _decoder );
        }
        else
        {
            bufferId = this->createBuffer_( _decoder );
        }

        if( bufferId == 0 )
        {
            return false;
        }

        m_decoder = _decoder;
        m_bufferId = bufferId;
        m_duration = dataInfo->duration;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WebAudioSoundBuffer::createBuffer_( const SoundDecoderInterfacePtr & _decoder )
    {
        const SoundCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

        if( dataInfo->bits != sizeof( int16_t ) )
        {
            LOGGER_ERROR( "invalid PCM16 sound sample bytes: %u"
                , dataInfo->bits
            );

            return 0;
        }

        size_t size = dataInfo->size;

        MemoryInterfacePtr memory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid sound memory: %zu"
            , size
        );

        void * buffer = memory->getBuffer();

        SoundDecoderData data;
        data.buffer = buffer;
        data.size = size;

        size_t decodeSize = _decoder->decode( &data );

        size_t frameSize = dataInfo->channels * sizeof( int16_t );
        uint32_t frames = (uint32_t)(decodeSize / frameSize);

        if( frames == 0 )
        {
            LOGGER_ERROR( "invalid sound decode: %zu"
                , decodeSize
            );

            return 0;
        }

        uint32_t bufferId = mengineAudioBufferCreate( buffer, frames, dataInfo->channels, dataInfo->frequency );

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WebAudioSoundBuffer::createStream_( const SoundDecoderInterfacePtr & _decoder )
    {
        const ContentInterfacePtr & content = _decoder->getContent();

        MemoryInterfacePtr memory = content->createMemoryFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid sound stream memory '%s'"
            , Helper::getContentFullPath( content ).c_str()
        );

        const void * data = memory->getBuffer();
        size_t size = memory->getSize();

        const SoundCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

        uint32_t bufferId = mengineAudioStreamCreate( data, size, dataInfo->duration );

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundBuffer::acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundBuffer::releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundBuffer::updateSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & WebAudioSoundBuffer::getDecoder() const
    {
        return m_decoder;
    }
    //////////////////////////////////////////////////////////////////////////
}
