#	include "MarmaladeSoundBufferBase.h"

#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBufferBase::MarmaladeSoundBufferBase()
        : m_format(0)
        , m_frequency(0)
        , m_channels(0)
        , m_time_total(0.f)
        , m_isStereo(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBufferBase::~MarmaladeSoundBufferBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBufferBase::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		// determine data parameters
		const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->length;
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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBufferBase::play( ALenum _source, bool _looped, float _pos )
	{
        (void)_source;
        (void)_looped;
        (void)_pos;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBufferBase::pause( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBufferBase::stop( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBufferBase::getTimePos( ALenum _source, float & _pos ) const
	{
        (void)_source;
        (void)_pos;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBufferBase::isStereo() const
	{
		return m_isStereo;
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundBufferBase::getTimeTotal() const
	{
		return m_time_total;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBufferBase::update()
	{
		// do nothing
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
