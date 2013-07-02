#	include "OALSoundBufferBase.h"

#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::OALSoundBufferBase()
        : m_format(0)
        , m_frequency(0)
        , m_channels(0)
        , m_length(0.f)
        , m_isStereo(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::~OALSoundBufferBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		// determine data parameters
		const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_length = dataInfo->length;

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
	bool OALSoundBufferBase::play( ALenum _source, bool _looped, float _pos )
	{
        (void)_source;
        (void)_looped;
        (void)_pos;
		// nothing to do

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::pause( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::stop( ALenum _source )
	{
        (void)_source;
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::getTimePos( ALenum _source, float & _pos ) const
	{
        (void)_source;
        (void)_pos;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::isStereo() const
	{
		return m_isStereo;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferBase::getTimeTotal() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::update()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
