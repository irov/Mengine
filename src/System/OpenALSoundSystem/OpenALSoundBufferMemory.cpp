#include "OpenALSoundBufferMemory.h"
#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Kernel/MemoryHelper.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferMemory::OALSoundBufferMemory()
		: m_alBufferId(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferMemory::~OALSoundBufferMemory()
	{
		if( m_alBufferId != 0 )
		{
			m_soundSystem->releaseBufferId( m_alBufferId );
			m_alBufferId = 0;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundBufferMemory::update()
    {
        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		m_alBufferId = m_soundSystem->genBufferId();

		if( m_alBufferId == 0 )
		{
			LOGGER_ERROR("OALSoundBuffer::load invalid gen buffer"
				);

			return false;
		}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_length = dataInfo->length;
		size_t size = dataInfo->size;
		
		MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( size, __FILE__, __LINE__ );

		if( binary_buffer == nullptr )
		{
			LOGGER_ERROR("OALSoundBuffer::load invalid sound %d memory %d"
				, size
				);

			return false;
		}

		void * binary_memory = binary_buffer->getBuffer();
		
		size_t decode_size = m_soundDecoder->decode( binary_memory, size );
        
		if( decode_size == 0 )
		{
			LOGGER_ERROR("OALSoundBuffer::load invalid sound %d decode %d"
				, size
				);

			return false;
		}
		
		decode_size -= decode_size % 4;
		
		if( m_channels == 1 )
		{
			m_format = AL_FORMAT_MONO16;
			m_isStereo = false;
		}
		else
		{
			m_format = AL_FORMAT_STEREO16;
			m_isStereo = true;
		}

		ALsizei al_decode_size = (ALsizei)decode_size;
		alBufferData( m_alBufferId, m_format, binary_memory, al_decode_size, m_frequency );
		
		if( OAL_CHECK_ERROR() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferMemory::play( ALuint _source, bool _looped, float _pos )
	{
        ALint state = 0;
        alGetSourcei( _source, AL_SOURCE_STATE, &state );
        OAL_CHECK_ERROR();

        if( state == AL_PLAYING )
        {
            //alSourceStop( _source );
            alSourceRewind( _source );
            OAL_CHECK_ERROR();
        }
        
		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();

		alSourcei( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );
		OAL_CHECK_ERROR();

        alSourcei( _source, AL_BUFFER, m_alBufferId );
        OAL_CHECK_ERROR();

        float al_pos = _pos * 0.001f;
		alSourcef( _source, AL_SEC_OFFSET, al_pos );
		OAL_CHECK_ERROR();

		alSourcePlay( _source );
		OAL_CHECK_ERROR();

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundBufferMemory::resume( ALuint _source )
    {
        alSourcePlay( _source );
        OAL_CHECK_ERROR();

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferMemory::pause( ALuint _source )
	{
		alSourcePause( _source );
		OAL_CHECK_ERROR();

		//alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		//OAL_CHECK_ERROR();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferMemory::stop( ALuint _source )
	{		
		alSourceStop( _source );
		OAL_CHECK_ERROR();
		
		{
			ALint val;

			do 
			{
				alGetSourcei( _source, AL_SOURCE_STATE, &val );
			} 
			while( val == AL_PLAYING );
		}

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();

        alSourceRewind( _source );
		OAL_CHECK_ERROR();	
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferMemory::setTimePos( ALuint _source, float _pos ) const
	{
		float al_pos = _pos * 0.001f;
		alSourcef( _source, AL_SEC_OFFSET, al_pos );
		
		if( OAL_CHECK_ERROR() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferMemory::getTimePos( ALuint _source, float & _pos ) const
	{
		float al_pos = 0.f;

		alGetSourcef( _source, AL_SEC_OFFSET, &al_pos );
		
        if( OAL_CHECK_ERROR() == false )
        {
            return false;
        }

        float pos = al_pos * 1000.f;

        float total = this->getTimeTotal();

        if( pos > total )
        {
            pos = total;
        }

        _pos = pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	
