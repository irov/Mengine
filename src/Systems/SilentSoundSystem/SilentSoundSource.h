#pragma once

#include "Interface/SoundSystemInterface.h"

#include "SilentSoundBuffer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SilentSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class SilentSoundSource
        : public SoundSourceInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SilentSoundSource );

    public:
        SilentSoundSource();
        ~SilentSoundSource() override;

    public:
        void initialize( SilentSoundSystem * _soundSystem );

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
        float getPosition() const override;
        bool setPosition( float _posMs ) override;

        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        const SoundBufferInterfacePtr & getSoundBuffer() const override;

    public:
        void setHeadMode( bool _headMode );
        bool getHeadMode() const;

    protected:
        SilentSoundSystem * m_soundSystem;
        
        bool m_headMode;
        bool m_playing;
        bool m_pausing;
        float m_volume;
        bool m_loop;

        uint32_t m_sourceId;
        float m_time;

        SilentSoundBufferPtr m_soundBuffer;        

    protected:
        void unloadBuffer_();
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SilentSoundSource, SoundSourceInterface> SilentSoundSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}
