#   pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "SilentSoundBuffer.h"

namespace Menge
{
	class SilentSoundSystem;	

	class SilentSoundSource
		: public SoundSourceInterface
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
		bool isPlaying() const override;

		void setVolume( float _volume ) override;
		float getVolume() const override;

        void setPosition( const mt::vec3f & _pos ) override;
		const mt::vec3f & getPosition() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		bool setPosMs( float _posMs ) override;

		void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
		SoundBufferInterfacePtr getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

	private:
		bool m_headMode;
		bool m_playing;
        mt::vec3f m_position;
		float m_volume;
		bool m_loop;		

		size_t m_sourceId;
		float m_timing;

		SilentSoundBufferPtr m_soundBuffer;		
		SilentSoundSystem * m_soundSystem;

    private:
        void apply_( size_t _source );
		void unloadBuffer_();
	};

	typedef stdex::intrusive_ptr<SilentSoundSource> SilentSoundSourcePtr;
}	// namespace Menge
