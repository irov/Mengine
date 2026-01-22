#pragma once

#include "ImGUIInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImGUIRenderProvider
        : public ImGUIRenderProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( ImGUIRenderProvider );

    public:
        ImGUIRenderProvider();
        ~ImGUIRenderProvider() override;

    protected:
        ImTextureID getImTexture( const RenderTextureInterfacePtr & _texture ) const override;

    protected:
        void newFrame() override;
        void endFrame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImGUIRenderProvider, ImGUIRenderProviderInterface> ImGUIRenderProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
