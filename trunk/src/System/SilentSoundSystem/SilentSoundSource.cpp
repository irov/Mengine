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
        , m_position(0.f, 0.f, 0.f)
		, m_loop(false)
		, m_soundBuffer(nullptr)
		, m_soundSystem(nullptr)
		, m_sourceId(0)
		, m_timing(0.f)
	{
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
	bool SilentSoundSource::play()
	{
		if( m_playing == true || m_soundBuffer == NULL )
		{
			return false;
		}

		m_sourceId = m_soundSystem->genSourceId();

		if( m_sourceId == 0 )
		{
			return false;
		}

		m_soundBuffer->play( m_sourceId, m_loop, m_timing );
	
		m_playing = true;

        return true;
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
	void SilentSoundSource::setPosition( const mt::vec3f & _pos )
	{
        m_position = _pos;
	}
	//////////////////////////////////////////////////////////////////////////
    const mt::vec3f & SilentSoundSource::getPosition() const 
	{
		return m_position;
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
		if( m_soundBuffer == nullptr )
		{
			return 0.f;
		}

        float ms = m_soundBuffer->getTimeTotal();

		return ms;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundSource::getPosMs() const
	{
		if( m_soundBuffer == nullptr )
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
        		
		return posms;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::setPosMs( float _posMs )
	{
		if( m_playing == true )
		{
			this->stop();
			m_timing = _posMs;
			this->play();
		}
		else
		{
			m_timing = _posMs;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = stdex::intrusive_static_cast<SilentSoundBufferPtr>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != nullptr && m_playing == true )
		{
			this->stop();
		}

		m_soundBuffer = nullptr;
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
	SoundBufferInterfacePtr SilentSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
