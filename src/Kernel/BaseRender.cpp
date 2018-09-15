#include "Kernel/BaseRender.h"

#include "Interface/RenderServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseRender::BaseRender()
        : m_externalRender( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseRender::~BaseRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_renderViewport = _viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & BaseRender::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_renderCamera = _camera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & BaseRender::getRenderCamera() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        m_renderScissor = _scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & BaseRender::getRenderScissor() const
    {
        return m_renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderTarget( const RenderTargetInterfacePtr & _target )
    {
        m_renderTarget = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & BaseRender::getRenderTarget() const
    {
        return m_renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::render( const RenderContext * _context )
    {
        this->_render( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_renderTarget( const RenderContext * _context )
    {
        (void)_context;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setExternalRender( bool _externalRender )
    {
        if( m_externalRender == _externalRender )
        {
            return;
        }

        m_externalRender = _externalRender;

        this->_setExternalRender( _externalRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setExternalRender( bool _externalRender )
    {
        (void)_externalRender;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderMesh( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , uint32_t _indexCount ) const
    {
        const RenderViewportInterfacePtr & viewport = _state->viewport;
        const RenderCameraInterfacePtr & camera = _state->camera;
        const RenderTransformationInterfacePtr & transformation = _state->transformation;
        const RenderScissorInterfacePtr & scissor = _state->scissor;
        const RenderTargetInterfacePtr & target = _state->target;

        RENDER_SERVICE()
            ->addRenderMesh( viewport, camera, transformation, scissor, target, _material, _vertexBuffer, _indexBuffer, _indexCount );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderObject( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
        , const RenderProgramVariableInterfacePtr & _variable
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const RenderIndex * _indices, uint32_t _indexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        const RenderViewportInterfacePtr & viewport = _state->viewport;
        const RenderCameraInterfacePtr & camera = _state->camera;
        const RenderTransformationInterfacePtr & transformation = _state->transformation;
        const RenderScissorInterfacePtr & scissor = _state->scissor;
        const RenderTargetInterfacePtr & target = _state->target;

        RENDER_SERVICE()
            ->addRenderObject( viewport, camera, transformation, scissor, target, _material, _variable, _vertices, _vertexCount, _indices, _indexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderQuad( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        const RenderViewportInterfacePtr & viewport = _state->viewport;
        const RenderCameraInterfacePtr & camera = _state->camera;
        const RenderTransformationInterfacePtr & transformation = _state->transformation;
        const RenderScissorInterfacePtr & scissor = _state->scissor;
        const RenderTargetInterfacePtr & target = _state->target;

        RENDER_SERVICE()
            ->addRenderQuad( viewport, camera, transformation, scissor, target, _material, _vertices, _vertexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderLine( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        const RenderViewportInterfacePtr & viewport = _state->viewport;
        const RenderCameraInterfacePtr & camera = _state->camera;
        const RenderTransformationInterfacePtr & transformation = _state->transformation;
        const RenderScissorInterfacePtr & scissor = _state->scissor;
        const RenderTargetInterfacePtr & target = _state->target;

        RENDER_SERVICE()
            ->addRenderLine( viewport, camera, transformation, scissor, target, _material, _vertices, _vertexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertex2D * BaseRender::getDebugRenderVertex2D( uint32_t _count ) const
    {
        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( _count );

        return vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & BaseRender::getDebugMaterial() const
    {
        const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugMaterial();

        return debugMaterial;
    }
}
