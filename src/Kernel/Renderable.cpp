#include "Kernel/Renderable.h"

#include "Interface/RenderSystemInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Renderable::Renderable()
        : m_hide( false )
        , m_localHide( false )
        , m_externalRender( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Renderable::~Renderable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_render( const RenderContext * _state )
    {
        (void)_state;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_debugRender( const RenderContext * _state )
    {
        (void)_state;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::setHide( bool _hide )
    {
        if( m_hide == _hide )
        {
            return;
        }

        m_hide = _hide;

        this->_setHide( _hide );
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_setHide( bool _hide )
    {
        (void)_hide;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::setLocalHide( bool _localHide )
    {
        if( m_localHide == _localHide )
        {
            return;
        }

        m_localHide = _localHide;

        this->_setLocalHide( _localHide );
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_setLocalHide( bool _localHide )
    {
        (void)_localHide;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::setExternalRender( bool _externalRender )
    {
        if( m_externalRender == _externalRender )
        {
            return;
        }

        m_externalRender = _externalRender;

        this->_setExternalRender( _externalRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_setExternalRender( bool _externalRender )
    {
        (void)_externalRender;
        //Empty
    }
    void Renderable::addRenderMesh( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
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
    void Renderable::addRenderObject( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
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
            ->addRenderObject( viewport, camera, transformation, scissor, target, _material, _vertices, _vertexCount, _indices, _indexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::addRenderQuad( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
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
    void Renderable::addRenderLine( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
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
    RenderVertex2D * Renderable::getDebugRenderVertex2D( uint32_t _count ) const
    {
        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( _count );

        return vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & Renderable::getDebugMaterial() const
    {
        const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugMaterial();

        return debugMaterial;
    }
}
