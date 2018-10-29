#pragma once

#include "ThreadTaskPrefetch.h"

#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    class ThreadTaskPrefetchSoundDecoder
        : public ThreadTaskPrefetch
    {
    public:
        ThreadTaskPrefetchSoundDecoder();
        ~ThreadTaskPrefetchSoundDecoder() override;

    public:
        void setSoundCodec( const ConstString & _soundCodec );
        const ConstString & getSoundCodec() const;

    public:
        const SoundDecoderInterfacePtr & getDecoder() const;

    protected:
        bool _onRun() override;
        bool _onMain() override;

    protected:
        ConstString m_soundCodec;

    protected:
        SoundDecoderInterfacePtr m_soundDecoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchSoundDecoder> ThreadTaskPrefetchSoundDecoderPtr;
    //////////////////////////////////////////////////////////////////////////
}