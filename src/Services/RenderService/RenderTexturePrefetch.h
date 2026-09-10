#pragma once

#include "Interface/RenderTexturePrefetchInterface.h"

#include "RenderTexturePrefetchJob.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTexturePrefetch
        : public RenderTexturePrefetchInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( RenderTexturePrefetch );

    public:
        RenderTexturePrefetch();
        ~RenderTexturePrefetch() override;

    public:
        ERenderTexturePrefetchState getState() const override;
        const ContentInterfacePtr & getContent() const override;
        RenderTextureInterfacePtr getTexture() const override;

    public:
        void initialize( const RenderTexturePrefetchJobPtr & _job, const ContentInterfacePtr & _content, const LambdaRenderTexturePrefetch & _callback );

    public:
        void cancel() override;
        void notify();
        void finalize();

    protected:
        RenderTexturePrefetchJobPtr m_job;
        ContentInterfacePtr m_content;
        LambdaRenderTexturePrefetch m_callback;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexturePrefetch, RenderTexturePrefetchInterface> RenderTexturePrefetchPtr;
    //////////////////////////////////////////////////////////////////////////
}
