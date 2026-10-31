#pragma once

#include "Interface/SoundSourceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class WebAudioSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( WebAudioSoundSource );

    public:
        WebAudioSoundSource();
        ~WebAudioSoundSource() override;

    public:
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
        bool play() override;
        void stop() override;
        void pause() override;
        bool resume() override;
        bool isPlay() const override;
        bool isPause() const override;
        bool isFinished() const override;

    protected:
        SoundBufferInterfacePtr m_soundBuffer;
        uint32_t m_sourceId;
        float m_volume;
        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<WebAudioSoundSource> WebAudioSoundSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}
