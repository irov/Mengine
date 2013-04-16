#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class SilentSoundSystem;
	class SilentSoundBuffer;

	class SilentSoundSource
		: public SoundSourceInterface
	{
	public:
		SilentSoundSource();
		~SilentSoundSource();

	public:
		void initialize( SilentSoundSystem * _soundSystem );

	public:
		void play() override;
		void pause() override;
		void stop() override;
		bool isPlaying() const override;

		void setVolume( float _volume ) override;
		float getVolume() const override;

		void setPosition( float _x, float _y, float _z ) override;
		const float * getPosition() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		void setPosMs( float _posMs ) override;

		void loadBuffer( SoundBufferInterface* _soundBuffer ) override;
		SoundBufferInterface* getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

	private:
		bool m_headMode;
		bool m_playing;
		float m_position[3];
		float m_volume;
		bool m_loop;		

		size_t m_sourceId;
		float m_timing;

		SilentSoundBuffer * m_soundBuffer;		
		SilentSoundSystem * m_soundSystem;

    private:
        void apply_( size_t _source );
		void unloadBuffer_();
	};
}	// namespace Menge
