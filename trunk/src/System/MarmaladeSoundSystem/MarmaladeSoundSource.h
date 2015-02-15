#   pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBuffer.h"
#	include "MarmaladeSoundFilter.h"

#	include <s3eSound.h>

namespace Menge
{
	class MarmaladeSoundSystem;
	
	struct MarmaladeSoundMemoryDesc;

	class MarmaladeSoundSource
		: public SoundSourceInterface
	{
	public:
		MarmaladeSoundSource();
		~MarmaladeSoundSource();
        
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem * _soundSystem );

	public:
		bool play() override;
		void pause() override;
		void stop() override;

	public:
		bool isPlay() const override;
		bool isPause() const override;		

	public:
		void setVolume( float _volume ) override;
		float getVolume() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		bool setPosMs( float _posMs ) override;

		void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
		SoundBufferInterfacePtr getSoundBuffer() const override;

	public:
		void complete();

    private:
        ServiceProviderInterface * m_serviceProvider;
		MarmaladeSoundSystem * m_soundSystem;

		SoundBufferInterfacePtr m_soundBuffer;

		float m_volume;
		bool m_playing;
		bool m_pausing;
		bool m_loop;

		float m_position;

		uint32_t m_soundId;
				
		//int32 m_filterBufferPos;
		//int16 m_filterBufferL[MARMALADE_SOUND_NUM_COEFFICIENTS];
		//int16 m_filterBufferR[MARMALADE_SOUND_NUM_COEFFICIENTS];
		//double m_filterCoefficients[MARMALADE_SOUND_NUM_COEFFICIENTS];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MarmaladeSoundSource> MarmaladeSoundSourcePtr;
}	// namespace Menge
