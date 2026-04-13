#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "WASAPISoundBufferBase.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class WASAPISoundBufferStream
        : public WASAPISoundBufferBase
    {
        DECLARE_FACTORABLE( WASAPISoundBufferStream );

    public:
        WASAPISoundBufferStream();
        ~WASAPISoundBufferStream() override;

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
        uint32_t renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition ) override;

    public:
        bool updateSoundBuffer() override;

    protected:
        bool prebuffer_();
        bool decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written );
        bool ensureDecodeFrames_( uint32_t _requiredFrames );

        void resetRingBuffer_();
        void resetDecodeBuffer_();
        void writeMixerFrames_( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames, float _gain ) const;

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
        AtomicUInt32 m_playPositionBytes;
        AtomicUInt32 m_activeRenders;

        size_t m_ringBufferSize;

        ThreadMutexInterfacePtr m_mutexDecoder;
        ThreadMutexInterfacePtr m_mutexUpdating;
        MemoryInterfacePtr m_memory;
        uint8_t * m_ringBuffer;

        MemoryInterfacePtr m_memoryDecode;
        uint8_t * m_decodeBuffer;
        size_t m_decodeBufferSize;
        uint32_t m_decodeFrames;
        double m_decodeCursor;
        bool m_decodeEOF;
        double m_resampleStep;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<WASAPISoundBufferStream, SoundBufferInterface> WASAPISoundBufferStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
