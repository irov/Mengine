#	include "SilentSoundSource.h"

#	include "SilentSoundSystem.h"
#	include "SilentSoundBuffer.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSource::SilentSoundSource()
		: m_headMode(true)
		, m_playing(false)
		, m_volume(1.f)
		, m_loop(false)
		, m_soundBuffer(NULL)
		, m_soundSystem(0)
		, m_sourceId(0)
		, m_timing(0.f)
	{
		m_position[0] = 0.f;
		m_position[1] = 0.f;
		m_position[2] = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSource::~SilentSoundSource()
	{
		this->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::initialize( SilentSoundSystem * _soundSystem )
	{
		m_soundSystem = _soundSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::play()
	{
		if( m_playing == true || m_soundBuffer == NULL )
		{
			return;
		}

		m_sourceId = m_soundSystem->genSourceId();

		if( m_sourceId == 0 )
		{
			return;
		}

		m_soundBuffer->play( m_sourceId, m_loop, m_timing );
	
		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::pause()
	{
		if( m_playing == false || m_soundBuffer == NULL )
		{
			return;
		}

		m_playing = false;

		if( m_sourceId != 0 )
		{
			m_timing = m_soundBuffer->getTimePos( m_sourceId );
			m_soundBuffer->stop( m_sourceId );
			m_soundSystem->releaseSourceId( m_sourceId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::stop()
	{
		if( m_playing == false )
		{
			return;
		}

		m_playing = false;

		if( m_sourceId != 0 )
		{
			m_soundBuffer->stop( m_sourceId );
			m_soundSystem->releaseSourceId( m_sourceId );
		}

		m_timing = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::isPlaying() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setPosition( float _x, float _y, float _z )
	{
        (void)_x;
        (void)_y;
        (void)_z;
        //Enpty
	}
	//////////////////////////////////////////////////////////////////////////
	const float * SilentSoundSource::getPosition() const 
	{
		return &(m_position[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setLoop( bool _loop )
	{
		m_loop = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::getLoop() const 
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundSource::getLengthMs() const
	{
		if( m_soundBuffer != NULL )
		{
			return m_soundBuffer->getTimeTotal() * 1000.0f;
		}

		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundSource::getPosMs() const
	{
		if( m_soundBuffer == NULL )
		{
			return 0.f;
		}

		if( m_sourceId == 0 )
		{
			return 0.f;
		}
			
		float posms = m_soundBuffer->getTimePos( m_sourceId );
		
		//timing dont assign to zero when m_soundBuffer is stopped!
		if( fabsf(posms) < 0.0001f && fabsf(m_timing) > 0.0001 )
		{ 
			posms = m_timing;
		}

		posms *= 1000.f;
		
		return posms;		
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setPosMs( float _posMs )
	{
		if( m_playing == true )
		{
			this->stop();
			m_timing = _posMs * 0.001f;
			this->play();
		}
		else
		{
			m_timing = _posMs * 0.001f;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::loadBuffer( SoundBufferInterface* _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = static_cast<SilentSoundBuffer*>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != NULL && m_playing == true )
		{
			this->stop();
		}

		m_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setHeadMode( bool _headMode )
	{
		m_headMode = _headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::getHeadMode() const
	{
		return m_headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* SilentSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
