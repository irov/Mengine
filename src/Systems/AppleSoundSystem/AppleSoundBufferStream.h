#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "AppleSoundBufferBase.h"

#include "Config/Atomic.h"

#include <AudioToolbox/AudioToolbox.h>

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

        uint32_t getReadableBytes_() const;
        uint32_t getWritableBytes_() const;

    protected:
        AtomicBool m_looped;
        AtomicBool m_updating;
        AtomicBool m_finished;
        AtomicUInt32 m_playCursorBytes;

        // SPSC ring buffer counters — separated to avoid false sharing
        // Writer (prebuffer_ / update thread) only writes m_writeCount
        // Reader (renderMixerFrames / audio thread) only writes m_readCount
        alignas(64) AtomicUInt32 m_writeCount;
        alignas(64) AtomicUInt32 m_readCount;

        AtomicFloat m_basePositionMs;

#if defined(MENGINE_DEBUG)
        AtomicUInt32 m_renderCalls;
        AtomicUInt32 m_updateTicks;
        AtomicUInt32 m_lastRenderFramesRequested;
        AtomicUInt32 m_lastRenderFramesProduced;
        AtomicUInt32 m_lastRenderBufferCount;
        AtomicUInt32 m_lastRenderBuffer0Channels;
        AtomicUInt32 m_lastRenderBuffer1Channels;
        AtomicUInt32 m_lastRenderBuffer0Size;
        AtomicUInt32 m_lastRenderBuffer1Size;
        AtomicBool m_loggedRenderLayout;
        AtomicBool m_loggedRenderMissing;
        AtomicBool m_loggedUnderflow;
        AtomicBool m_renderObservedUnderflow;
#endif

        uint32_t m_ringBufferSizeMask;
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
