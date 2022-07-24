#pragma once

#include "ThreadTaskPrefetch.h"

#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    class ThreadTaskPrefetchSoundDecoder
        : public ThreadTaskPrefetch
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchSoundDecoder );

    public:
        ThreadTaskPrefetchSoundDecoder();
        ~ThreadTaskPrefetchSoundDecoder() override;

    public:
        void setSoundCodec( const ConstString & _soundCodec );
        const ConstString & getSoundCodec() const;

    public:
        const SoundDecoderInterfacePtr & getDecoder() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        ConstString m_soundCodec;

    protected:
        SoundDecoderInterfacePtr m_soundDecoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchSoundDecoder, ThreadTaskInterface> ThreadTaskPrefetchSoundDecoderPtr;
    //////////////////////////////////////////////////////////////////////////
}