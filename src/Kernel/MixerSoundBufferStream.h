#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/MixerSoundBufferBase.h"
#include "Kernel/SpinLock.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerSoundBufferStream
        : public MixerSoundBufferBase
    {
        DECLARE_FACTORABLE( MixerSoundBufferStream );

    public:
        MixerSoundBufferStream();
        ~MixerSoundBufferStream() override;

    public:
        bool setTimePosition( float _position ) override;
        bool getTimePosition( float * const _position ) const override;
        bool setLoopSource( bool _looped ) override;

    public:
        void setVolume( float _gain ) override;

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
        uint32_t renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition ) override;

    public:
        bool updateSoundBuffer() override;
        bool isEndOfStream() const override;

    protected:
        void releaseStream_();
        bool prepareSource_( bool _looped, float _position );
        void stopSource_();
        bool seekSource_( float _position );
        bool setLoopSource_( bool _looped );

        bool prebuffer_();
        bool seekPosition_( float _position );
        bool decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written );
        bool ensureDecodeFrames_( uint32_t _requiredFrames, uint32_t * const _availableFrames );

        void resetRingBuffer_();
        void resetDecodeBuffer_();
        void writeMixerFrames_( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames, float _gain ) const;

    protected:
        SpinLock m_renderLock;

        AtomicBool m_looped;
        AtomicBool m_updating;
        AtomicBool m_finished;
        AtomicUInt32 m_readOffset;
        AtomicUInt32 m_writeOffset;
        AtomicUInt32 m_availableBytes;
        AtomicUInt64 m_playPositionBytes;

        size_t m_ringBufferSize;

        ThreadMutexInterfacePtr m_mutexDecoder;
        ThreadMutexInterfacePtr m_mutexUpdating;
        MemoryInterfacePtr m_memory;
        uint8_t * m_ringBuffer;

        MemoryInterfacePtr m_memoryDecode;
        uint8_t * m_decodeBuffer;
        size_t m_decodeBufferSize;
        uint32_t m_decodeFrames;
        uint64_t m_decodePhase;
        bool m_decodeEOF;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerSoundBufferStream, SoundBufferInterface> MixerSoundBufferStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
