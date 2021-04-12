#pragma once

#include "Interface/ImageCodecInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskPrefetchImageDecoder
        : public ThreadTaskPrefetch
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchImageDecoder );

    public:
        ThreadTaskPrefetchImageDecoder();
        ~ThreadTaskPrefetchImageDecoder() override;

    public:
        void setImageCodec( const ConstString & _codec );
        const ConstString & getImageCodec() const;

    public:
        const ImageDecoderInterfacePtr & getDecoder() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskMain() override;

    protected:
        ConstString m_codecType;

        ImageDecoderInterfacePtr m_imageDecoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchImageDecoder, ThreadTaskInterface> ThreadTaskPrefetchImageDecoderPtr;
    //////////////////////////////////////////////////////////////////////////
}