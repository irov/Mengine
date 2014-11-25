#   pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBuffer.h"
#	include "MarmaladeSoundFilter.h"

#	include <s3eSound.h>

namespace Menge
{
	class MarmaladeSoundSystem;

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
		static int32 s_AudioCallback( void * _sys, void * _user );
		static int32 s_EndSampleCallback( void * _sys, void * _user );
		static int32 s_StopAudioCallback( void * _sys, void * _user );

		int32 audioCallback( s3eSoundGenAudioInfo * _info );
		void endSampleCallback( s3eSoundEndSampleInfo * _info );
		void stopAudioCallback( s3eSoundEndSampleInfo * _info );

	private:
		void checkMemoryCache_();
		
    private:
        ServiceProviderInterface * m_serviceProvider;
		MarmaladeSoundSystem * m_soundSystem;

		MarmaladeSoundBufferPtr m_soundBuffer;

		float m_volume;
		bool m_playing;
		bool m_pausing;
		bool m_loop;

		int m_soundChannel;
				
		int32 m_filterBufferPos;
		//int16 m_filterBufferL[MARMALADE_SOUND_NUM_COEFFICIENTS];
		//int16 m_filterBufferR[MARMALADE_SOUND_NUM_COEFFICIENTS];
		//double m_filterCoefficients[MARMALADE_SOUND_NUM_COEFFICIENTS];

		volatile uint8 m_volume_s3e;
		volatile bool m_stereo_s3e;
		volatile uint32 m_carriage_s3e;
		volatile uint32 m_W;
		volatile uint32 m_L;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MarmaladeSoundSource> MarmaladeSoundSourcePtr;
}	// namespace Menge
