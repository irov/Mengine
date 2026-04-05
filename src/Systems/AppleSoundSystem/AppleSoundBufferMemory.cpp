#include "AppleSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferMemory::AppleSoundBufferMemory()
        : m_pcmDataSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferMemory::~AppleSoundBufferMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::_releaseSoundBuffer()
    {
        m_pcmMemory = nullptr;
        m_soundDecoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;
        size_t size = dataInfo->size;

        MemoryInterfacePtr pcmMemory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( pcmMemory, "invalid sound memory: %zu"
            , size
        );

        void * pcmBuffer = pcmMemory->getBuffer();

        SoundDecoderData data;
        data.buffer = pcmBuffer;
        data.size = size;

        size_t decodeSize = _soundDecoder->decode( &data );

        if( decodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode: %zu"
                , size
            );

            return false;
        }

        switch( m_channels )
        {
        case 1:
            {
                m_isStereo = false;
            }break;
        case 2:
            {
                m_isStereo = true;
            }break;
        default:
            {
                LOGGER_WARNING( "invalid sound format channels: %u"
                    , m_channels
                );

                return false;
            }break;
        }

        size_t frameSize = m_channels * 2;
        size_t correctDecodeSize = decodeSize - decodeSize % frameSize;

        m_soundDecoder = _soundDecoder;
        m_pcmMemory = pcmMemory;
        m_pcmDataSize = correctDecodeSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::playSource( bool _looped, float _position )
    {
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::stopSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::pauseSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::resumeSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::setTimePosition( float _position )
    {
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::getTimePosition( float * const _position ) const
    {
        *_position = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & AppleSoundBufferMemory::getPCMMemory() const
    {
        return m_pcmMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferMemory::getFrameCount() const
    {
        if( m_channels == 0 )
        {
            return 0;
        }

        return (uint32_t)(m_pcmDataSize / this->getFrameSize());
    }
    //////////////////////////////////////////////////////////////////////////
}
