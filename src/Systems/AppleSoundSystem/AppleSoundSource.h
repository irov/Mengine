#pragma once

#include "Interface/SoundSystemInterface.h"

#include "AppleSoundBufferBase.h"

#include "Kernel/Factorable.h"
#include "Kernel/SpinLock.h"

#include "Config/Atomic.h"

#include <AudioToolbox/AudioToolbox.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AppleSoundSource );

    protected:
        enum
        {
            INVALID_BUS_INDEX = 0xFFFFFFFFu
        };

    public:
        AppleSoundSource();
        ~AppleSoundSource() override;

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
        OSStatus renderMixerFrames( UInt32 _frames, AudioBufferList * _ioData );

    protected:
        void unloadBuffer_();

    protected:
        AppleSoundBufferBasePtr getSoundBuffer_() const;

        bool acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain );
        void releaseSourceBus_();
        uint32_t detachBusIndex_();

        void beginMutableState_();
        void endMutableState_();
        bool tryEnterRender_();
        void leaveRender_();

    protected:
        mutable SpinLock m_lock;

        AtomicFloat m_volume;
        AtomicUInt32 m_framePosition;

        AppleSoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        AtomicBool m_playing;
        AtomicBool m_pausing;
        AtomicBool m_loop;
        AtomicBool m_finished;
        AtomicBool m_renderBarrier;
        AtomicUInt32 m_activeRenders;
        uint32_t m_busIndex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSoundSource, SoundSourceInterface> AppleSoundSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}
