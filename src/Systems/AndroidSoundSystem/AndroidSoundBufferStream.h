#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "AndroidSoundBufferBase.h"

#include "Config/Atomic.h"

#ifndef MENGINE_ANDROID_STREAM_BUFFER_COUNT
#define MENGINE_ANDROID_STREAM_BUFFER_COUNT (4)
#endif

#ifndef MENGINE_ANDROID_STREAM_BUFFER_SIZE
#define MENGINE_ANDROID_STREAM_BUFFER_SIZE (44100 * 2)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundBufferStream
        : public AndroidSoundBufferBase
    {
        DECLARE_FACTORABLE( AndroidSoundBufferStream );

    public:
        AndroidSoundBufferStream();
        ~AndroidSoundBufferStream() override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

    public:
        bool playSource( bool _looped, float _position ) override;
        void stopSource() override;
        void pauseSource() override;
        void resumeSource() override;

    public:
        bool setTimePosition( float _position ) override;
        bool getTimePosition( float * const _position ) const override;

    public:
        void setVolume( float _gain ) override;

    public:
        bool updateSoundBuffer() override;

    protected:
        bool fillBuffer_( uint32_t _bufferIndex, size_t * const _bytes );

    protected:
        static void bufferQueueCallback_( SLAndroidSimpleBufferQueueItf _bq, void * _context );
        void onBufferConsumed_();

    protected:
        void setUpdating_( bool _updating );

    protected:
        uint8_t * m_bufferData[MENGINE_ANDROID_STREAM_BUFFER_COUNT];
        uint32_t m_currentBuffer;

        AtomicBool m_looped;
        AtomicBool m_updating;
        AtomicUInt32 m_consumedCount;

        ThreadMutexInterfacePtr m_mutexUpdating;
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSoundBufferStream, SoundBufferInterface> AndroidSoundBufferStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
