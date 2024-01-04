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
        const ImageDecoderInterfacePtr & getDecoder() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        ImageDecoderInterfacePtr m_imageDecoder;
        MemoryInputInterfacePtr m_memoryInput;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchImageDecoder, ThreadTaskInterface> ThreadTaskPrefetchImageDecoderPtr;
    //////////////////////////////////////////////////////////////////////////
}