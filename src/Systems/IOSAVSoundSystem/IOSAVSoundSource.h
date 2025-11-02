#pragma once

#include "Interface/SoundSourceInterface.h"

#include "Kernel/Factorable.h"

#include "IOSAVSoundBuffer.h"

namespace Mengine
{
    class IOSAVSoundSystem;

    class IOSAVSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( IOSAVSoundSource );

    public:
        IOSAVSoundSource();
        ~IOSAVSoundSource() override;

    public:
        void initialize( IOSAVSoundSystem * _soundSystem, bool _headMode );

    public:
        bool play() override;
        void stop() override;
        void pause() override;
        bool resume() override;

    public:
        bool isPlay() const override;
        bool isPause() const override;

    public:
        void setVolume( float _volume ) override;
        float getVolume() const override;

        void setLoop( bool _loop ) override;
        bool getLoop() const override;

        float getDuration() const override;
        bool setPosition( float _position ) override;
        float getPosition() const override;

    public:
        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        const SoundBufferInterfacePtr & getSoundBuffer() const override;

    protected:
        void ensurePlayer_();
        void releasePlayer_();
        void releaseCurrentBuffer_();

    protected:
        IOSAVSoundSystem * m_soundSystem;
        IOSAVSoundBufferPtr m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_paused;
        bool m_loop;
        float m_volume;
        float m_position;
        float m_startPosition;

        void * m_playerNode;
        void * m_currentBuffer;
    };

    typedef IntrusivePtr<IOSAVSoundSource, SoundSourceInterface> IOSAVSoundSourcePtr;
}

