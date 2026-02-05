#include "AndroidSoundBuffer.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBuffer::AndroidSoundBuffer()
        : m_frequency( 0 )
        , m_channels( 0 )
        , m_bits( 0 )
        , m_duration( 0.f )
        , m_streamable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBuffer::~AndroidSoundBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBuffer::acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBuffer::releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBuffer::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & AndroidSoundBuffer::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        if( dataInfo == nullptr )
        {
            LOGGER_ERROR( "AndroidSoundBuffer invalid codec data info" );

            return false;
        }

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_bits = dataInfo->bits;
        m_duration = dataInfo->duration;
        m_streamable = _streamable;

        if( m_channels == 0 || m_channels > 2 )
        {
            LOGGER_ERROR( "AndroidSoundBuffer unsupported channel count %u", m_channels );

            return false;
        }

        if( m_bits != 8 && m_bits != 16 )
        {
            LOGGER_ERROR( "AndroidSoundBuffer unsupported bits %u", m_bits );

            return false;
        }

        size_t pcmSize = dataInfo->size;

        if( pcmSize == 0 )
        {
            LOGGER_ERROR( "AndroidSoundBuffer invalid data size" );

            return false;
        }

        MemoryInterfacePtr pcmMemory = Helper::createMemoryCacheBuffer( pcmSize, MENGINE_DOCUMENT_FACTORABLE );

        if( pcmMemory == nullptr )
        {
            LOGGER_ERROR( "AndroidSoundBuffer invalid memory allocate size %zu", pcmSize );

            return false;
        }

        SoundDecoderData data;
        data.buffer = pcmMemory->getBuffer();
        data.size = pcmSize;

        size_t decodeSize = m_soundDecoder->decode( &data );

        if( decodeSize == 0 )
        {
            LOGGER_ERROR( "AndroidSoundBuffer invalid decode size" );

            return false;
        }

        m_pcmData.assign( (uint8_t *)data.buffer, (uint8_t *)data.buffer + decodeSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const uint8_t * AndroidSoundBuffer::getData() const
    {
        if( m_pcmData.empty() == true )
        {
            return nullptr;
        }

        return m_pcmData.data();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidSoundBuffer::getDataSize() const
    {
        return m_pcmData.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AndroidSoundBuffer::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AndroidSoundBuffer::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AndroidSoundBuffer::getBits() const
    {
        return m_bits;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundBuffer::getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBuffer::isStreamable() const
    {
        return m_streamable;
    }
    //////////////////////////////////////////////////////////////////////////
}

