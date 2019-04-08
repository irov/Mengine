#pragma once

#include "Interface/SoundSystemInterface.h"

#include "OpenALSoundBufferBase.h"

#include "AL/al.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
    public:
        OpenALSoundSource();
        ~OpenALSoundSource() override;

    public:
        void setSoundSystem( OpenALSoundSystem* _soundSystem );
        OpenALSoundSystem* getSoundSystem() const;

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
        float getPosition() const override;
        bool setPosition( float _posMs ) override;

        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        const SoundBufferInterfacePtr & getSoundBuffer() const override;

    public:
        void setHeadMode( bool _headMode );
        bool getHeadMode() const;

    protected:
        void unloadBuffer_();
        void releaseSourceId_();

    protected:
        void apply_( ALuint _source );

    protected:
        OpenALSoundSystem * m_soundSystem;

        float m_volume;

        ALuint m_sourceId;
        float m_time;

        OpenALSoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_pausing;
        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundSource> OpenALSoundSourcePtr;
}
