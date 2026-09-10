#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "PrefetchedRenderImageLoader.h"
#include "Kernel/ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskPrefetchTexture
        : public ThreadTask
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchTexture );

    public:
        ThreadTaskPrefetchTexture();
        ~ThreadTaskPrefetchTexture() override;

    public:
        const PrefetchedRenderImageLoaderPtr & getImage() const;

    public:
        void initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags );

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ContentInterfacePtr m_content;
        uint32_t m_codecFlags;
        PrefetchedRenderImageLoaderPtr m_image;
        MemoryBufferInterfacePtr m_pixels;
        FileGroupInterface * m_realFileGroup;
        InputStreamInterfacePtr m_stream;
        MemoryInputInterfacePtr m_encoded;
        ImageDecoderInterfacePtr m_decoder;
        bool m_streamOpened;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchTexture, ThreadTaskInterface> ThreadTaskPrefetchTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}
