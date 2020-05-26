#include "OpenALSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::_acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::_release()
    {
        if( m_alBufferId != 0 )
        {
            m_soundSystem->releaseBufferId( m_alBufferId );
            m_alBufferId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::update()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        m_alBufferId = m_soundSystem->genBufferId();

        if( m_alBufferId == 0 )
        {
            LOGGER_ERROR( "invalid gen buffer"
            );

            return false;
        }

        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_length = dataInfo->length;
        size_t size = dataInfo->size;

        MemoryInterfacePtr binary_memory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( binary_memory, false, "invalid sound %u [memory %zu]"
            , m_alBufferId
            , size
        );

        void * binary_memory_buffer = binary_memory->getBuffer();

        size_t decode_size = m_soundDecoder->decode( binary_memory_buffer, size );

        if( decode_size == 0 )
        {
            LOGGER_ERROR( "invalid sound %u decode %zu"
                , m_alBufferId
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
            LOGGER_ERROR( "invliad format channels '%d'"
                , m_channels
            );

            return false;
        }

        ALsizei al_decode_size = (ALsizei)decode_size;
        IF_OPENAL_CALL( alBufferData, (m_alBufferId, m_format, binary_memory_buffer, al_decode_size, m_frequency) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::playSource( ALuint _source, bool _looped, float _pos )
    {
        ALint state = 0;
        OPENAL_CALL( alGetSourcei, (_source, AL_SOURCE_STATE, &state) );

        if( state == AL_PLAYING )
        {
            OPENAL_CALL( alSourceRewind, (_source) );
            OPENAL_CALL( alSourcei, (_source, AL_BUFFER, 0) );
        }

        OPENAL_CALL( alSourcei, (_source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE) );

        OPENAL_CALL( alSourcei, (_source, AL_BUFFER, m_alBufferId) );

        if( _pos > 0.f )
        {
            float al_pos = _pos * 0.001f;
            OPENAL_CALL( alSourcef, (_source, AL_SEC_OFFSET, al_pos) );
        }

        OPENAL_CALL( alSourcePlay, (_source) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::resumeSource( ALuint _source )
    {
        OPENAL_CALL( alSourcePlay, (_source) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::pauseSource( ALuint _source )
    {
        OPENAL_CALL( alSourcePause, (_source) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferMemory::stopSource( ALuint _source )
    {
        OPENAL_CALL( alSourceStop, (_source) );

        {
            ALint val;

            do
            {
                OPENAL_CALL( alGetSourcei, (_source, AL_SOURCE_STATE, &val) );
            } while( val == AL_PLAYING );
        }

        OPENAL_CALL( alSourceRewind, (_source) );
        OPENAL_CALL( alSourcei, (_source, AL_BUFFER, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::setTimePos( ALuint _source, float _pos ) const
    {
        float al_pos = _pos * 0.001f;
        IF_OPENAL_CALL( alSourcef, (_source, AL_SEC_OFFSET, al_pos) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferMemory::getTimePos( ALuint _source, float * _pos ) const
    {
        float al_pos = 0.f;

        IF_OPENAL_CALL( alGetSourcef, (_source, AL_SEC_OFFSET, &al_pos) )
        {
            return false;
        }

        float pos = al_pos * 1000.f;

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
