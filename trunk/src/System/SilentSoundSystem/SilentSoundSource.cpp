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
		, m_pausing(false)
		, m_volume(1.f)
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
		if( m_playing == true )
		{
			return true;
		}

		if( m_soundBuffer == nullptr )
		{
			return false;
		}

		if( m_pausing == false )
		{
			m_sourceId = m_soundSystem->genSourceId();

			if( m_sourceId == 0 )
			{
				return false;
			}

			m_soundBuffer->play( m_sourceId, m_loop, m_timing );
		}
		else
		{
			m_soundBuffer->resume( m_sourceId );
		}
	
		m_playing = true;
		m_pausing = false;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::pause()
	{
		if( m_playing == false )
		{
			return;
		}

		if( m_pausing == true )
		{
			return;
		}

		if( m_soundBuffer == nullptr )
		{
			return;
		}

		if( m_sourceId == 0 )
		{
			return;
		}

		m_playing = false;
		m_pausing = true;

		m_timing = m_soundBuffer->getTimePos( m_sourceId );

		m_soundBuffer->pause( m_sourceId );		
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSource::stop()
	{
		if( m_playing == false && m_pausing == false )
		{
			return;
		}

		m_playing = false;
		m_pausing = false;

		if( m_sourceId != 0 )
		{
			uint32_t sourceId = m_sourceId;
			m_sourceId = 0;

			m_soundBuffer->stop( sourceId );
			m_soundSystem->releaseSourceId( sourceId );
		}

		m_timing = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::isPlay() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSource::isPause() const
	{
		return m_pausing;
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

		if( m_pausing == true )
		{
			return m_timing;
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
			return false;
		}

		m_timing = _posMs;
		
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
