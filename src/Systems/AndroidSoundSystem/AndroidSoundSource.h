#pragma once

#include "Interface/SoundSystemInterface.h"

#include "AndroidSoundBufferBase.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AndroidSoundSource );

    public:
        AndroidSoundSource();
        ~AndroidSoundSource() override;

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

    protected:
        void unloadBuffer_();

    protected:
        float m_volume;
        float m_time;

        AndroidSoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_pausing;
        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSoundSource, SoundSourceInterface> AndroidSoundSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}
