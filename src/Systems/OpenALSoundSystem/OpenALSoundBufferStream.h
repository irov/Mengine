#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "OpenALSoundBufferMemory.h"

#include "Config/Atomic.h"

#ifndef MENGINE_OPENAL_STREAM_BUFFER_COUNT
#define MENGINE_OPENAL_STREAM_BUFFER_COUNT (4)
#endif

#ifndef MENGINE_OPENAL_STREAM_BUFFER_SIZE
#define MENGINE_OPENAL_STREAM_BUFFER_SIZE (44100 * 2)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundBufferStream
        : public OpenALSoundBufferBase
    {
        DECLARE_FACTORABLE( OpenALSoundBufferStream );

    public:
        OpenALSoundBufferStream();
        ~OpenALSoundBufferStream() override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

    public:
        bool playSource( ALuint _source, bool _looped, float _pos ) override;
        bool resumeSource( ALuint _source ) override;
        void pauseSource( ALuint _source ) override;
        void stopSource( ALuint _source ) override;

    public:
        bool setTimePos( ALuint _source, float _pos ) const override;
        bool getTimePos( ALuint _source, float * const _pos ) const override;

    public:
        bool updateSoundBuffer() override;

    protected:
        bool bufferData_( ALuint _alBufferId, size_t * const _bytes );

        void removeBuffers_();

    protected:
        ALuint m_alBuffersId[MENGINE_OPENAL_STREAM_BUFFER_COUNT] = {0};

        ALuint m_sourceId;

        AtomicBool m_looped;
        AtomicBool m_updating;

        ThreadMutexInterfacePtr m_mutexUpdating;
        MemoryInterfacePtr m_memory;

    protected:
        void setUpdating_( bool _updating );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundBufferStream, SoundBufferInterface> OpenALSoundBufferStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
