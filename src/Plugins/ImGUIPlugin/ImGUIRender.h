#pragma once

#include "ImGUIInterface.h"

#include "Interface/RenderExternalInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImGUIRender
        : public Node
        , public RenderExternalInterface
        , public UnknownImGUIRenderInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( ImGUIRender );
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        ImGUIRender();
        ~ImGUIRender() override;

    public:
        void setProvider( const LambdaImGUIProvider & _provider ) override;
        const LambdaImGUIProvider & getProvider() const override;

    public:
        void setImGUIRenderProvider( const ImGUIRenderProviderInterfacePtr & _renderProvider );
        const ImGUIRenderProviderInterfacePtr & getImGUIRenderProvider() const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void onRenderExternal() const override;

    protected:
        LambdaImGUIProvider m_provider;

        ImGUIRenderProviderInterfacePtr m_renderProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ImGUIRender> ImGUIRenderPtr;
    //////////////////////////////////////////////////////////////////////////
}