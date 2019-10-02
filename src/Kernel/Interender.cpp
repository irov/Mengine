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
}