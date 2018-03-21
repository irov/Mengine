#include "SilentSoundBuffer.h"

#include "Interface/SoundCodecInterface.h"

#include <time.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	static float s_getTimeMs()
	{
		clock_t cl = clock();
		float sec = (float)(cl) / float(CLOCKS_PER_SEC);

		return sec;
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundBuffer::SilentSoundBuffer()
        : m_playTime(0.f)
        , m_pauseTime(0.f)
        , m_frequency(0)
        , m_channels(0)
        , m_length(0.f)
        , m_isStereo(false)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundBuffer::~SilentSoundBuffer()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::update()
    {
        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	const SoundDecoderInterfacePtr & SilentSoundBuffer::getDecoder() const
	{
		return m_soundDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_length = dataInfo->length;
		//size_t size = dataInfo->size;

		if( m_channels == 1 )
		{
			m_isStereo = false;
		}
		else
		{
			m_isStereo = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundBuffer::play( uint32_t _id, bool _looped, float _pos )
	{
        (void)_id;
        (void)_looped;
        (void)_pos;

        float sec = s_getTimeMs();

        m_playTime = sec;
        m_pauseTime = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundBuffer::resume( uint32_t _id )
	{
		(void)_id;

		float sec = s_getTimeMs();
		float deltha = m_pauseTime - m_playTime;

		m_playTime = sec - deltha;
		m_pauseTime = 0.f;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundBuffer::pause( uint32_t _id )
	{
        (void)_id;
		        
        float sec = s_getTimeMs();

        m_pauseTime = sec;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundBuffer::stop( uint32_t _id )
	{
        (void)_id;

        m_playTime = 0.f;
        m_pauseTime = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundBuffer::getTimePos( uint32_t _id ) const
	{
        (void)_id;

        if( m_pauseTime > m_playTime )
        {
            return m_pauseTime;
        }

        float sec = s_getTimeMs();

		return sec - m_playTime;
	}
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimeTotal() const
    {
        return m_length;
    }
	//////////////////////////////////////////////////////////////////////////
}	
