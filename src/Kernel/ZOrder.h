#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/DummyPicker.h"
#include "Kernel/BaseTransformation.h"

#include "ZOrderRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ZOrder
        : public Node
        , protected BaseRender
        , protected DummyPicker
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        ZOrder();
        ~ZOrder() override;

    public:
        const RenderInterfacePtr & getZOrderRender( int32_t _zOrder );

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;

    protected:
        typedef Vector<ZOrderRenderPtr> VectorZOrderRenders;
        VectorZOrderRenders m_zOrderRenders;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ZOrder> ZOrderPtr;
    //////////////////////////////////////////////////////////////////////////
}