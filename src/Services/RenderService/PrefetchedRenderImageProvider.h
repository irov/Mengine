#pragma once

#include "Interface/RenderImageProviderInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrefetchedRenderImageProvider
        : public RenderImageProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PrefetchedRenderImageProvider );

    public:
        PrefetchedRenderImageProvider();
        ~PrefetchedRenderImageProvider() override;

    public:
        void initialize( const ContentInterfacePtr & _content, const RenderImageLoaderInterfacePtr & _loader );

    public:
        const ContentInterfacePtr & getContent() const override;
        RenderImageLoaderInterfacePtr getLoader( const DocumentInterfacePtr & _doc ) const override;

    protected:
        ContentInterfacePtr m_content;
        RenderImageLoaderInterfacePtr m_loader;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrefetchedRenderImageProvider, RenderImageProviderInterface> PrefetchedRenderImageProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
