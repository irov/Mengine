#	pragma once

#	include "Interface/SoundSystemInterface.h"
#	include "Interface/SoundCodecInterface.h"

#	include <s3eSound.h>

namespace Menge
{
	class MarmaladeSoundBuffer
		: public SoundBufferInterface
	{
	public:
		MarmaladeSoundBuffer();
		virtual ~MarmaladeSoundBuffer();

	public:
		void setServiceProvider( ServiceProviderInterface * m_serviceProvider );

	public:
		uint32_t getFrequency(){ return m_frequency; }
		uint32_t getChannels(){ return m_channels; }
		uint32_t getSamples(){ return m_samples; }
		uint32_t getBits(){ return m_bits; }
		float getLength(){ return m_length; }
		bool getStereo(){ return m_stereo; }

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder );

	public:
		bool update() override;

	public:
		bool rewind() override;

	public:
		const SoundDecoderInterfacePtr & getDecoder() const;
        
	protected:
		ServiceProviderInterface * m_serviceProvider;
		SoundDecoderInterfacePtr m_soundDecoder;

		uint32_t m_frequency;
		uint32_t m_channels;
		uint32_t m_samples;
		uint32_t m_bits;
		float m_length;

		bool m_stereo;
	};

	typedef stdex::intrusive_ptr<MarmaladeSoundBuffer> MarmaladeSoundBufferPtr;
}	// namespace Menge
