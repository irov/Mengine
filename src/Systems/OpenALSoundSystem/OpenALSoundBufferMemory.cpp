#include "OpenALSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "OpenALSoundSystem.h"
#include "OpenALSoundErrorHelper.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferMemory::OpenALSoundBufferMemory()
        : m_alBufferId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferMemory::~OpenALSoundBufferMemory()
    {
        MENGINE_ASSERTION_FATAL( m_alBufferId == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::_releaseSoundBuffer()
    {
        if( m_alBufferId != 0 )
        {
            m_soundSystem->releaseBufferId( m_alBufferId );
            m_alBufferId = 0;
        }

        m_soundDecoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        MENGINE_ASSERTION_FATAL( m_alBufferId == 0 );

        ALuint alBufferId = m_soundSystem->genBufferId();

        if( alBufferId == 0 )
        {
            LOGGER_ASSERTION( "invalid gen buffer" );

            return false;
        }

        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_length = dataInfo->length;
        size_t size = dataInfo->size;

        MemoryInterfacePtr binary_memory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( binary_memory, "invalid sound %u [memory %zu]"
            , alBufferId
            , size
        );

        void * binary_memory_buffer = binary_memory->getBuffer();

        SoundDecoderData data;
        data.buffer = binary_memory_buffer;
        data.size = size;

        size_t decode_size = m_soundDecoder->decode( &data );

        if( decode_size == 0 )
        {
            m_soundSystem->releaseBufferId( alBufferId );

            LOGGER_ASSERTION( "invalid sound %u decode %zu"
                , alBufferId
                , size
            );

            return false;
        }

        decode_size -= decode_size % 4;

        switch( m_channels )
        {
        case 1:
            {
                m_format = AL_FORMAT_MONO16;
                m_isStereo = false;
            }break;
        case 2:
            {
                m_format = AL_FORMAT_STEREO16;
                m_isStereo = true;
            }break;
        default:
            {
                m_soundSystem->releaseBufferId( alBufferId );

                LOGGER_ASSERTION( "invalid format channels [%u]"
                    , m_channels
                );

                return false;
            }break;
        }

        ALsizei al_decode_size = (ALsizei)decode_size;
        ALsizei al_frequency = (ALsizei)m_frequency;
        MENGINE_IF_OPENAL_CALL( alBufferData, (alBufferId, m_format, binary_memory_buffer, al_decode_size, al_frequency) )
        {
            m_soundSystem->releaseBufferId( alBufferId );

            return false;
        }

        m_alBufferId = alBufferId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::playSource( ALuint _source, bool _looped, float _pos )
    {
        ALint state = 0;
        MENGINE_OPENAL_CALL( alGetSourcei, (_source, AL_SOURCE_STATE, &state) );

        if( state == AL_PLAYING )
        {
            MENGINE_OPENAL_CALL( alSourceRewind, (_source) );
            MENGINE_OPENAL_CALL( alSourcei, (_source, AL_BUFFER, 0) );
        }

        MENGINE_OPENAL_CALL( alSourcei, (_source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE) );

        MENGINE_OPENAL_CALL( alSourcei, (_source, AL_BUFFER, m_alBufferId) );

        if( _pos > 0.f )
        {
            ALfloat al_pos = (ALfloat)(_pos * 0.001f);
            MENGINE_OPENAL_CALL( alSourcef, (_source, AL_SEC_OFFSET, al_pos) );
        }

        MENGINE_OPENAL_CALL( alSourcePlay, (_source) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::resumeSource( ALuint _source )
    {
        MENGINE_OPENAL_CALL( alSourcePlay, (_source) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::pauseSource( ALuint _source )
    {
        MENGINE_OPENAL_CALL( alSourcePause, (_source) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::stopSource( ALuint _source )
    {
        MENGINE_OPENAL_CALL( alSourceStop, (_source) );

        {
            ALint val;

            do
            {
                MENGINE_OPENAL_CALL( alGetSourcei, (_source, AL_SOURCE_STATE, &val) );
            } while( val == AL_PLAYING );
        }

        MENGINE_OPENAL_CALL( alSourceRewind, (_source) );
        MENGINE_OPENAL_CALL( alSourcei, (_source, AL_BUFFER, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::setTimePos( ALuint _source, float _pos ) const
    {
        ALfloat al_pos = (ALfloat)(_pos * 0.001f);
        MENGINE_IF_OPENAL_CALL( alSourcef, (_source, AL_SEC_OFFSET, al_pos) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::getTimePos( ALuint _source, float * const _pos ) const
    {
        ALfloat al_pos = 0.f;
        MENGINE_IF_OPENAL_CALL( alGetSourcef, (_source, AL_SEC_OFFSET, &al_pos) )
        {
            return false;
        }

        float pos = (float)al_pos * 1000.f;

        float total = this->getTimeTotal();

        if( pos > total )
        {
            pos = total;
        }

        *_pos = pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
