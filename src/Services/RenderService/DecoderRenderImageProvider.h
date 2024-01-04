#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DecoderRenderImageProvider
        : public RenderImageProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DecoderRenderImageProvider );

    public:
        DecoderRenderImageProvider();
        ~DecoderRenderImageProvider() override;

    public:
        void initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags );

    public:
        RenderImageLoaderInterfacePtr getLoader( const DocumentInterfacePtr & _doc ) const override;

    protected:
        ContentInterfacePtr m_content;
        uint32_t m_codecFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageProvider> DecoderRenderImageProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
