#	include "SDLSoundBuffer.h"

#	include "SDLSoundError.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SDLSoundBuffer::SDLSoundBuffer()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SDLSoundBuffer::~SDLSoundBuffer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLSoundBuffer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		//const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

		//m_frequency = dataInfo->frequency;
		//m_channels = dataInfo->channels;
		//m_samples = dataInfo->size / dataInfo->channels;
		//m_length = dataInfo->length;
		//m_stereo = dataInfo->stereo;
		//m_bits = dataInfo->bits;

		m_soundDecoder = _soundDecoder;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const SoundDecoderInterfacePtr & SDLSoundBuffer::getDecoder() const
	{
		return m_soundDecoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundBuffer::update()
	{
		// do nothing
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
