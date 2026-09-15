#pragma once

#include "MosaicInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MosaicRender
        : public Node
        , public UnknownMosaicRenderInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( MosaicRender );
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        MosaicRender();
        ~MosaicRender() override;

    public:
        void setProvider( const LambdaMosaicProvider & _provider ) override;
        const LambdaMosaicProvider & getProvider() const override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        LambdaMosaicProvider m_provider;

        MosaicProviderId m_providerId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<MosaicRender> MosaicRenderPtr;
    //////////////////////////////////////////////////////////////////////////
}
