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
		bool load( const SoundDecoderInterfacePtr & _soundDecoder );

	public:
		bool update() override;

	public:
		const SoundDecoderInterfacePtr & getDecoder() const override;
        
	protected:
		ServiceProviderInterface * m_serviceProvider;
		SoundDecoderInterfacePtr m_soundDecoder;
	};

	typedef stdex::intrusive_ptr<MarmaladeSoundBuffer> MarmaladeSoundBufferPtr;
}	// namespace Menge
