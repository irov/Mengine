#pragma once

#include "Interface/SoundSourceInterface.h"

#include "AndroidSoundBuffer.h"

#include "Kernel/Factorable.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Mengine
{
    class AndroidSoundSystem;

    class AndroidSoundSource
        : public SoundSourceInterface
    {
        DECLARE_FACTORABLE( AndroidSoundSource );

    public:
        AndroidSoundSource();
        ~AndroidSoundSource() override;

    public:
        void initialize( AndroidSoundSystem * _soundSystem );
        void finalize();

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

    public:
        void setLoop( bool _loop ) override;
        bool getLoop() const override;

    public:
        float getDuration() const override;

    public:
        bool setPosition( float _position ) override;
        float getPosition() const override;

    public:
        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        const SoundBufferInterfacePtr & getSoundBuffer() const override;

    public:
        void setHeadMode( bool _headMode );
        bool getHeadMode() const;

    public:
        void updateFromSystem();

    protected:
        bool createPlayer_();
        void destroyPlayer_();
        void applyVolume_();
        void applyLooping_();
        bool enqueueBuffer_( size_t _offsetBytes );

    protected:
        void onBufferQueueEnd_();
        static void s_callbackBufferQueue_( SLAndroidSimpleBufferQueueItf _bufferQueue, void * _context );

    protected:
        AndroidSoundSystem * m_soundSystem;
        AndroidSoundBufferPtr m_soundBuffer;

        float m_volume;
        bool m_loop;
        bool m_headMode;

        bool m_playing;
        bool m_pausing;
        float m_position;

        bool m_needRestart;

        SLObjectItf m_playerObject;
        SLPlayItf m_playerPlay;
        SLAndroidSimpleBufferQueueItf m_playerBufferQueue;
        SLVolumeItf m_playerVolume;
    };

    typedef IntrusivePtr<AndroidSoundSource, SoundSourceInterface> AndroidSoundSourcePtr;
}

