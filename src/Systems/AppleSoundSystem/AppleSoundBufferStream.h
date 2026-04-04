#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "AppleSoundBufferBase.h"

#include "Config/Atomic.h"

#include <AudioUnit/AudioUnit.h>

#ifndef MENGINE_APPLE_STREAM_BUFFER_COUNT
#define MENGINE_APPLE_STREAM_BUFFER_COUNT (4)
#endif

#ifndef MENGINE_APPLE_STREAM_BUFFER_SIZE
#define MENGINE_APPLE_STREAM_BUFFER_SIZE (44100 * 2)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundBufferStream
        : public AppleSoundBufferBase
    {
        DECLARE_FACTORABLE( AppleSoundBufferStream );

    public:
        AppleSoundBufferStream();
        ~AppleSoundBufferStream() override;

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
        bool renderMixerFrames( AudioBufferList * _ioData, UInt32 _frames, UInt32 * const _renderedFrames );

    public:
        bool updateSoundBuffer() override;

    protected:
        bool prebuffer_();
        bool decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written );
        void resetRingBuffer_();
        void writeMixerFrames_( AudioBufferList * _ioData, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames ) const;

        void beginMutableState_();
        void endMutableState_();
        bool tryEnterRender_();
        void leaveRender_();

    protected:
        AtomicBool m_looped;
        AtomicBool m_updating;
        AtomicBool m_finished;
        AtomicBool m_renderBarrier;
        AtomicUInt32 m_readOffset;
        AtomicUInt32 m_writeOffset;
        AtomicUInt32 m_availableBytes;
        AtomicUInt32 m_playCursorBytes;
        AtomicUInt32 m_activeRenders;
        AtomicFloat m_basePositionMs;

        size_t m_ringBufferSize;

        ThreadMutexInterfacePtr m_mutexDecoder;
        ThreadMutexInterfacePtr m_mutexUpdating;
        MemoryInterfacePtr m_memory;
        uint8_t * m_ringBuffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSoundBufferStream, SoundBufferInterface> AppleSoundBufferStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
