#	include "MarmaladeSoundBuffer.h"
#	include "MarmaladeSoundSystem.h"

#	include "Interface/SoundCodecInterface.h"

#	include "MarmaladeError.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBuffer::MarmaladeSoundBuffer()
		: m_serviceProvider(nullptr)
        , m_soundSystem(nullptr)
		, m_alBufferId(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBuffer::~MarmaladeSoundBuffer()
	{
		if( m_alBufferId != 0 )
		{
			m_soundSystem->releaseBufferId( m_alBufferId );
			m_alBufferId = 0;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundBuffer::initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem * _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
        m_soundSystem = _soundSystem;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		m_alBufferId = m_soundSystem->genBufferId();		

		if( m_alBufferId == 0 )
		{
			// TODO: report in case of error
			//printf( "Error: %s\n", alGetString( error ) );
			return false;
		}

		const SoundCodecDataInfo* dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;
		size_t size = dataInfo->size;

        //printf("OALSoundBuffer::load %.4f\n"
        //    , float(size) / 1024.f
        //    );

		unsigned char* buffer = new unsigned char[ size /*+ fixed_sound_buffer_size*/ ];
		unsigned int decode_size = _soundDecoder->decode( buffer, size );

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

		alBufferData( m_alBufferId, m_format, buffer, decode_size, m_frequency );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		delete[] buffer;		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBuffer::play( ALenum _source, bool _looped, float _pos )
	{
		ALint state = 0;
		alGetSourcei( _source, AL_SOURCE_STATE, &state );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		if( state == AL_PLAYING )
		{
			alSourceStop( _source );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcei( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcei( _source, AL_BUFFER, m_alBufferId );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_SEC_OFFSET, _pos );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcePlay( _source );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBuffer::pause( ALenum _source )
	{
		alSourcePause( _source );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBuffer::stop( ALenum _source )
	{
		alSourceStop( _source );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBuffer::getTimePos( ALenum _source, float & _pos ) const
	{
		float pos = 0.f;

		alGetSourcef( _source, AL_SEC_OFFSET, &pos );
		
        if( MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider) == true )
        {
            return false;
        }

        _pos = pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
