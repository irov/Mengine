#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/DummyPicker.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Interender
        : public Node
        , protected BaseRender
        , protected DummyPicker
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Interender );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Interender();
        ~Interender() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        const RenderResolutionInterfacePtr & getPickerResolution() const override;
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;
        const RenderTransformationInterfacePtr & getPickerTransformation() const override;
        const RenderTargetInterfacePtr & getPickerTarget() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Interender> InterenderPtr;
    //////////////////////////////////////////////////////////////////////////
}
