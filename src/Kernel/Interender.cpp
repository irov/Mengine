#include "Interender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Interender::Interender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Interender::~Interender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Interender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderResolutionInterfacePtr & Interender::getPickerResolution() const
    {
        const RenderResolutionInterfacePtr & resolution = this->getRenderResolution();

        return resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & Interender::getPickerViewport() const
    {
        const RenderViewportInterfacePtr & viewport = this->getRenderViewport();

        return viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & Interender::getPickerCamera() const
    {
        const RenderCameraInterfacePtr & camera = this->getRenderCamera();

        return camera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & Interender::getPickerScissor() const
    {
        const RenderScissorInterfacePtr & scissor = this->getRenderScissor();

        return scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & Interender::getPickerTransformation() const
    {
        const RenderTransformationInterfacePtr & transformation = this->getRenderTransformation();

        return transformation;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & Interender::getPickerTarget() const
    {
        const RenderTargetInterfacePtr & target = this->getRenderTarget();

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
}