#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

#include "Jewelry.h"

namespace Mengine
{
    class JewelryChain
        : public Node
        , public BaseRender
    {
        DECLARE_RENDERABLE();

    public:
        JewelryChain();
        ~JewelryChain() override;

    public:
        void addJewelry( const JewelryPtr & _jewelry );

    public:
        void clear();

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    public:
        typedef Vector<JewelryPtr> VectorJewelries;
        VectorJewelries m_jewelries;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryChain> JewelryChainPtr;
    //////////////////////////////////////////////////////////////////////////
}