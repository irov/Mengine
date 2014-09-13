#	pragma once

#	include "OALSoundBuffer.h"

namespace Menge
{
#   define OPENAL_STREAM_BUFFER_COUNT 2
#   define OPENAL_STREAM_BUFFER_SIZE (44100)

	class OALSoundBufferStream
		: public OALSoundBuffer
	{
	public:
		OALSoundBufferStream();
		~OALSoundBufferStream();

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

	public:
		bool play( ALenum _source, bool _looped, float _pos ) override;
        bool resume( ALenum _source ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;

	public:
		bool setTimePos( ALenum _source, float _pos ) const override;
		bool getTimePos( ALenum _source, float & _pos ) const override;

	public:
		bool update() override;

	public:
		bool rewind() override;

    protected:
        bool bufferData_( ALuint _alBufferId, size_t & _bytes );

        void removeBuffers_();
		
	protected:
		ALuint m_alBuffersId[OPENAL_STREAM_BUFFER_COUNT];

		SoundDecoderInterfacePtr m_soundDecoder;
		
		ALuint m_sourceId;

		bool m_updating;

	private:
		void setUpdating( bool _updating );
		bool getUpdating() const;
	};
	
}	// namespace Menge
