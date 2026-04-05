#pragma once

#include "Interface/SoundSystemInterface.h"

#include "WASAPISoundBufferBase.h"

#include "Kernel/Factorable.h"
#include "Kernel/SpinLock.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class WASAPISoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( WASAPISoundSource );

    protected:
        static constexpr uint32_t INVALID_BUS_INDEX = 0xFFFFFFFFu;

    public:
        WASAPISoundSource();
        ~WASAPISoundSource() override;

    public:
        bool initialize();
        void finalize();

    public:
        bool play() override;
        void stop() override;
        void pause() override;
        bool resume() override;

    public:
        bool isPlay() const override;
        bool isPause() const override;
        bool isFinished() const override;

        void setVolume( float _volume ) override;
        float getVolume() const override;

        void setLoop( bool _loop ) override;
        bool getLoop() const override;

        float getDuration() const override;

        bool setPosition( float _position ) override;
        float getPosition() const override;

        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        const SoundBufferInterfacePtr & getSoundBuffer() const override;

    public:
        void setHeadMode( bool _headMode );
        bool getHeadMode() const;

    public:
        void renderMixerFrames( uint32_t _frames, uint32_t _outputChannels, float * _mixBuffer, float _gain );

    protected:
        void unloadBuffer_();

    protected:
        WASAPISoundBufferBasePtr getSoundBuffer_() const;

        bool acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain );
        void releaseSourceBus_();
        uint32_t detachBusIndex_();

    protected:
        mutable SpinLock m_lock;

        AtomicFloat m_volume;
        AtomicUInt32 m_framePosition;

        WASAPISoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        AtomicBool m_playing;
        AtomicBool m_pausing;
        AtomicBool m_loop;
        AtomicBool m_finished;
        uint32_t m_busIndex;
    };

    typedef IntrusivePtr<WASAPISoundSource, SoundSourceInterface> WASAPISoundSourcePtr;
}
