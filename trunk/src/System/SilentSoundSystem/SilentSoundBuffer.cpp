#	include "SilentSoundBuffer.h"

#	include "Interface/SoundCodecInterface.h"

#   include <time.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SilentSoundBuffer::SilentSoundBuffer()
        : m_playTime(0.f)
        , m_pauseTime(0.f)
        , m_frequency(0)
        , m_channels(0)
        , m_time_total(0.f)
        , m_isStereo(false)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundBuffer::~SilentSoundBuffer()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::release()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::update()
    {
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundBuffer::load( SoundDecoderInterface * _soundDecoder )
	{
		const SoundCodecDataInfo* dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;
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
	void SilentSoundBuffer::play( size_t _id, bool _looped, float _pos )
	{
        (void)_id;
        (void)_looped;
        (void)_pos;

        clock_t cl = clock();
        float sec = (float)(cl) / float(CLOCKS_PER_SEC);

        m_playTime = sec;
        m_pauseTime = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundBuffer::pause( size_t _id )
	{
        (void)_id;

        clock_t cl = clock();
        float sec = (float)(cl) / float(CLOCKS_PER_SEC);

        m_pauseTime = sec;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundBuffer::stop( size_t _id )
	{
        (void)_id;

        m_playTime = 0.f;
        m_pauseTime = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	float SilentSoundBuffer::getTimePos( size_t _id ) const
	{
        (void)_id;

        if( m_pauseTime > m_playTime )
        {
            return m_pauseTime;
        }

        clock_t cl = clock();
        float sec = (float)(cl) / float(CLOCKS_PER_SEC);

		return sec - m_playTime;
	}
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimeTotal() const
    {
        return m_time_total;
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
