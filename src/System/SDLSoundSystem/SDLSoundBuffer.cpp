#	include "MarmaladeSoundBuffer.h"

#	include "MarmaladeSoundError.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBuffer::MarmaladeSoundBuffer()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundBuffer::~MarmaladeSoundBuffer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundBuffer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder )
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
	const SoundDecoderInterfacePtr & MarmaladeSoundBuffer::getDecoder() const
	{
		return m_soundDecoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundBuffer::update()
	{
		// do nothing
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
