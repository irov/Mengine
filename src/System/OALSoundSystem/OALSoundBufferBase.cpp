#	include "OALSoundBufferBase.h"

#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::OALSoundBufferBase()
		: m_serviceProvider(nullptr)
		, m_soundSystem(nullptr)	
		, m_format(0)
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
	void OALSoundBufferBase::initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem )
	{
		m_serviceProvider = _serviceProvider;
		m_soundSystem = _soundSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	const SoundDecoderInterfacePtr & OALSoundBufferBase::getDecoder() const
	{
		return m_soundDecoder;
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
}	// namespace Menge
