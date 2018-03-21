#pragma once

#include "Interface/SoundSystemInterface.h"

#include "SilentSoundBuffer.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class SilentSoundSystem;	

	class SilentSoundSource
		: public ServantBase<SoundSourceInterface>
	{
	public:
		SilentSoundSource();
		~SilentSoundSource();

	public:
		void initialize( SilentSoundSystem * _soundSystem );

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

		float getDuration() const override;
		float getPosition() const override;
		bool setPosition( float _posMs ) override;

		void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
		SoundBufferInterfacePtr getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

	private:
		bool m_headMode;
		bool m_playing;
		bool m_pausing;
		float m_volume;
		bool m_loop;		

		uint32_t m_sourceId;
		float m_timing;

		SilentSoundBufferPtr m_soundBuffer;		
		SilentSoundSystem * m_soundSystem;

    private:
        void apply_( uint32_t _source );
		void unloadBuffer_();
	};

	typedef stdex::intrusive_ptr<SilentSoundSource> SilentSoundSourcePtr;
}	
