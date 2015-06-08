#   pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "SDLSoundBuffer.h"
#	include "SDLSoundFilter.h"

namespace Menge
{
	class SDLSoundSystem;
	
	class SDLSoundSource
		: public SoundSourceInterface
	{
	public:
		SDLSoundSource();
		~SDLSoundSource();
        
    public:
		void initialize( ServiceProviderInterface * _serviceProvider, SDLSoundSystem * _soundSystem );

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
		SDLSoundSystem * m_soundSystem;

		SoundBufferInterfacePtr m_soundBuffer;

		float m_volume;
		bool m_playing;
		bool m_pausing;
		bool m_loop;

		float m_position;

		uint32_t m_soundId;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SDLSoundSource> SDLSoundSourcePtr;
}	// namespace Menge
