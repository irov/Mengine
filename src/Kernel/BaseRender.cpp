#include "Kernel/BaseRender.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/Assertion.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseRender::BaseRender()
        : m_relationRender( nullptr )
        , m_externalRender( false )
        , m_renderEnable( false )
        , m_hide( false )
        , m_localHide( false )
        , m_rendering( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseRender::~BaseRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRelationRender( RenderInterface * _relationRender )
    {
        if( m_relationRender != nullptr )
        {
            m_relationRender->removeRelationRenderChildren_( this );
        }

        m_relationRender = static_cast<BaseRender *>(_relationRender);

        if( m_relationRender != nullptr )
        {
            m_relationRender->addRelationRenderChildren_( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::removeRelationRender()
    {
        if( m_relationRender == nullptr )
        {
            return;
        }

        m_relationRender->removeRelationRenderChildren_( this );
        m_relationRender = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::foreachChildren( const LambdaRender & _lambda )
    {
        for( RenderInterface * child : m_relationRenderChildren )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderEnable( bool _renderEnable )
    {
        m_renderEnable = _renderEnable;

        this->updateRendering_();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setHide( bool _hide )
    {
        if( m_hide == _hide )
        {
            return;
        }

        m_hide = _hide;

        this->_setHide( _hide );

        this->updateRendering_();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setHide( bool _hide )
    {
        (void)_hide;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setLocalHide( bool _localHide )
    {
        if( m_localHide == _localHide )
        {
            return;
        }

        m_localHide = _localHide;

        this->_setLocalHide( _localHide );

        this->updateRendering_();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setLocalHide( bool _localHide )
    {
        (void)_localHide;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRelationRenderChildren_( BaseRender * _child )
    {
        m_relationRenderChildren.push_back( _child );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::removeRelationRenderChildren_( BaseRender * _child )
    {
        VectorBaseRender::iterator it_erase = std::find( m_relationRenderChildren.begin(), m_relationRenderChildren.end(), _child );

        MENGINE_ASSERTION( it_erase != m_relationRenderChildren.end(), ("remove relation child is not found") );

        m_relationRenderChildren.erase( it_erase );
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
    void BaseRender::renderWithChildren( const RenderContext * _context, bool _external )
    {
        if( this->isRendering() == false )
        {
            return;
        }

        if( this->isExternalRender() == true && _external == false )
        {
            return;
        }

        RenderContext context;

        const RenderViewportInterfacePtr & renderViewport = this->getRenderViewport();

        if( renderViewport != nullptr )
        {
            context.viewport = renderViewport;
        }
        else
        {
            context.viewport = _context->viewport;
        }

        const RenderCameraInterfacePtr & renderCamera = this->getRenderCamera();

        if( renderCamera != nullptr )
        {
            context.camera = renderCamera;
        }
        else
        {
            context.camera = _context->camera;
        }

        const RenderScissorInterfacePtr & renderScissor = this->getRenderScissor();

        if( renderScissor != nullptr )
        {
            context.scissor = renderScissor;
        }
        else
        {
            context.scissor = _context->scissor;
        }

        const RenderTargetInterfacePtr & renderTarget = this->getRenderTarget();

        if( renderTarget != nullptr )
        {
            context.target = renderTarget;
        }
        else
        {
            context.target = _context->target;
        }

        if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
        {
            this->render( &context );
        }

        for( BaseRender * child : m_relationRenderChildren )
        {
            child->renderWithChildren( &context, false );
        }

        if( context.target != nullptr )
        {
            const RenderInterfacePtr & targetRender = this->makeTargetRender( &context );

            if( targetRender != nullptr )
            {
                targetRender->render( _context );
            }
        }        
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & BaseRender::makeTargetRender( const RenderContext * _context )
    {
        const RenderInterfacePtr & render = this->_makeTargetRender( _context );

        return render;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & BaseRender::_makeTargetRender( const RenderContext * _context )
    {
        (void)_context;

        return RenderInterfacePtr::none();
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
    void BaseRender::addRenderMesh( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , uint32_t _indexCount ) const
    {
        RENDER_SERVICE()
            ->addRenderMesh( _context, _material, _vertexBuffer, _indexBuffer, _indexCount );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderObject( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
        , const RenderProgramVariableInterfacePtr & _variable
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const RenderIndex * _indices, uint32_t _indexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        RENDER_SERVICE()
            ->addRenderObject( _context, _material, _variable, _vertices, _vertexCount, _indices, _indexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderQuad( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        RENDER_SERVICE()
            ->addRenderQuad( _context, _material, _vertices, _vertexCount, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRenderLine( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug ) const
    {
        RENDER_SERVICE()
            ->addRenderLine( _context, _material, _vertices, _vertexCount, _bb, _debug );
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
    //////////////////////////////////////////////////////////////////////////
    const Color & BaseRender::getWorldColor() const
    {
        if( m_relationRender == nullptr )
        {
            const Color & localColor = Colorable::getLocalColor();

            return localColor;
        }

        if( Colorable::isInvalidateColor() == false )
        {
            const Color & relationColor = Colorable::getRelationColor();

            return relationColor;
        }

        const Color & parentColor = m_relationRender->getWorldColor();

        const Color & relationColor = Colorable::updateRelationColor( parentColor );

        return relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::calcTotalColor( Color & _color ) const
    {
        const Color & worldColour = this->getWorldColor();
        _color = worldColour;

        const Color & personalColour = this->getPersonalColor();
        _color *= personalColour;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseRender::isSolidColor() const
    {
        const Color & worldColour = this->getWorldColor();

        float worldAlpha = worldColour.getA();
        float personalAlpha = this->getPersonalAlpha();

        bool solid = mt::equal_f_f( worldAlpha * personalAlpha, 1.f );

        return solid;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::invalidateColor()
    {
        Colorable::invalidateColor();

        for( BaseRender * child : m_relationRenderChildren )
        {
            child->invalidateColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::updateRendering_()
    {
        if( this->isRenderEnable() == false )
        {
            m_rendering = false;

            return;
        }

        if( this->isHide() == true )
        {
            m_rendering = false;

            return;
        }

        if( this->isLocalTransparent() == true )
        {
            m_rendering = false;

            return;
        }

        m_rendering = true;
    }
}
