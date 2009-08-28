/*
 *	OALSoundSource.cpp
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundSource.h"

#	include "OALSoundSystem.h"
#	include "OALSoundBufferBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::OALSoundSource( OALSoundSystem* _soundSystem )
		: m_headMode( true )
		, m_playing( false )
		, m_volume( 1.0f )
		, m_looped( false )
		, m_soundBuffer( NULL )
		, m_soundSystem( _soundSystem )
		, m_alSourceName( 0 )
		, m_timing( 0.0f )
	{
		m_position[0] = 0.0f;
		m_position[1] = 0.0f;
		m_position[2] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::~OALSoundSource()
	{
		stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::play()
	{
		if( m_playing == true || m_soundBuffer == NULL )
		{
			return;
		}

		bool isStereo = m_soundBuffer->isStereo();
		m_alSourceName = m_soundSystem->popSource( isStereo );

		if( m_alSourceName != 0 )
		{
			apply_( m_alSourceName );
			m_soundBuffer->play( m_alSourceName, m_looped, m_timing );
		}

		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::pause()
	{
		if( m_playing == false || m_soundBuffer == NULL )
		{
			return;
		}
		m_playing = false;

		if( m_alSourceName != 0 )
		{
			m_timing = m_soundBuffer->getTimePos( m_alSourceName );
			m_soundBuffer->stop( m_alSourceName );
			alSourceRewind( m_alSourceName );
			ALenum error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::pause alSourceRewind Error: %s\n", alGetString( error ) );
			}
			m_soundSystem->pushSource( m_alSourceName, m_soundBuffer->isStereo() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::stop()
	{
		if( m_playing == false )
		{
			return;
		}
		m_playing = false;

		if( m_alSourceName != 0 )
		{
			m_soundBuffer->stop( m_alSourceName );
			alSourceRewind( m_alSourceName );
			ALenum error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::stop alSourceRewind Error: %s\n", alGetString( error ) );
			}
			m_soundSystem->pushSource( m_alSourceName, m_soundBuffer->isStereo() );
		}
		m_timing = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::isPlaying() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;
		if( m_playing == true && m_alSourceName != 0 )
		{
			alSourcef( m_alSourceName, AL_GAIN, m_volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setPosition( float _x, float _y, float _z )
	{
		m_position[0] = _x;
		m_position[1] = _y;
		m_position[2] = _z;
		if( m_playing == true && m_alSourceName != 0 )
		{
			alSourcefv( m_alSourceName, AL_POSITION, &(m_position[0]) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const float * OALSoundSource::getPosition() const 
	{
		return &(m_position[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setLooped( bool _loop )
	{
		m_looped = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::isLooped() const 
	{
		return m_looped;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getLengthMs()
	{
		if( m_soundBuffer != NULL )
		{
			return m_soundBuffer->getTimeTotal() * 1000.0f;
		}

		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getPosMs()
	{
		if( (m_soundBuffer != NULL) && (m_alSourceName != 0) )
		{
			return m_soundBuffer->getTimePos( m_alSourceName ) * 1000.0f;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setPosMs( float _posMs )
	{
		if( m_playing == true )
		{
			stop();
			m_timing = _posMs * 0.001f;
			play();
		}
		else
		{
			m_timing = _posMs * 0.001f;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::loadBuffer( SoundBufferInterface* _soundBuffer )
	{
		unloadBuffer_();

		m_soundBuffer = static_cast<OALSoundBufferBase*>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != NULL && m_playing == true )
		{
			stop();
		}
		m_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setHeadMode( bool _headMode )
	{
		m_headMode = _headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::getHeadMode() const
	{
		return m_headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::apply_( ALuint _source )
	{
		ALenum error = AL_NO_ERROR;
		if( m_headMode )
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_TRUE );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::apply_ AL_SOURCE_RELATIVE Error: %s\n", alGetString( error ) );
			}
			alSourcef( _source, AL_ROLLOFF_FACTOR, 0.0f );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::apply_ AL_ROLLOFF_FACTOR Error: %s\n", alGetString( error ) );
			}
			alSource3f( _source, AL_DIRECTION, 0.0f, 0.0f, 0.0f );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::apply_ AL_DIRECTION Error: %s\n", alGetString( error ) );
			}
		} 
		else 
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_FALSE );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::apply_ AL_SOURCE_RELATIVE Error: %s\n", alGetString( error ) );
			}
			alSourcef( _source, AL_ROLLOFF_FACTOR, 1.0f );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO error reporting
				printf( "OALSoundSource::apply_ AL_ROLLOFF_FACTOR Error: %s\n", alGetString( error ) );
			}
		}

		alSourcei( _source, AL_LOOPING, AL_FALSE );	
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "OALSoundSource::apply_ AL_LOOPING Error: %s\n", alGetString( error ) );
		}
		alSourcefv( _source, AL_POSITION, &(m_position[0]) );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "OALSoundSource::apply_ AL_POSITION Error: %s\n", alGetString( error ) );
		}
		alSourcef( _source, AL_MIN_GAIN, 0.0f );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "OALSoundSource::apply_ AL_MIN_GAIN Error: %s\n", alGetString( error ) );
		}
		alSourcef( _source, AL_MAX_GAIN, 1.0f );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "OALSoundSource::apply_ AL_MAX_GAIN Error: %s\n", alGetString( error ) );
		}
		alSourcef( _source, AL_GAIN, m_volume );

		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "OALSoundSource::apply_ AL_GAIN Error: %s\n", alGetString( error ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSource::getSoundBuffer()
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
