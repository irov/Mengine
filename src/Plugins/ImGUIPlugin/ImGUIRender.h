#pragma once

#include "ImGUIInterface.h"

#include "Interface/RenderExternalInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    class ImGUIRender
        : public Node
        , public BaseRender
        , public RenderExternalInterface
        , public ImGUIRenderProviderInterface
        , public UnknownImGUIRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_RENDERABLE();

    public:
        ImGUIRender();
        ~ImGUIRender() override;

    public:
        void setProvider( const LambdaImGUIProvider & _provider ) override;
        const LambdaImGUIProvider & getProvider() const override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void onRenderExternal() const override;

    protected:
        ImTextureID getImTexture( const RenderTextureInterfacePtr & _texture ) const override;

    protected:
        LambdaImGUIProvider m_provider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImGUIRender> ImGUIPtr;
    //////////////////////////////////////////////////////////////////////////
}