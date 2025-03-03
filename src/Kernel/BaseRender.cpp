#include "Kernel/BaseRender.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseRender::BaseRender()
        : m_relationRender( nullptr )
        , m_extraRelationRender( nullptr )
        , m_zGroup( MENGINE_RENDER_ZGROUP_DEFAULT )
        , m_zIndex( MENGINE_RENDER_ZINDEX_DEFAULT )
        , m_externalRender( false )
        , m_renderEnable( false )
        , m_hide( false )
        , m_localHide( false )
        , m_rendering( true )
        , m_invalidateRendering( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseRender::~BaseRender()
    {
        MENGINE_ASSERTION_FATAL( m_extraRelationRender == nullptr, "extra relation render is not nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRelationRender( RenderInterface * _relationRender )
    {
        MENGINE_ASSERTION_FATAL( _relationRender != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION_FATAL( _relationRender != this, "set this relation" );

        if( m_extraRelationRender != nullptr )
        {
            m_relationRender = static_cast<BaseRender *>(_relationRender);

            return;
        }

        if( m_relationRender != nullptr )
        {
            m_relationRender->removeRelationRenderChildren_( this );
        }

        m_relationRender = static_cast<BaseRender *>(_relationRender);
        m_relationRender->addRelationRenderChildrenBack_( this );

        this->invalidateColor();

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRelationRenderFront( RenderInterface * _relationRender )
    {
        MENGINE_ASSERTION_FATAL( _relationRender != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION_FATAL( _relationRender != this, "set this relation" );

        if( m_extraRelationRender != nullptr )
        {
            m_relationRender = static_cast<BaseRender *>(_relationRender);

            return;
        }

        if( m_relationRender != nullptr )
        {
            m_relationRender->removeRelationRenderChildren_( this );
        }

        m_relationRender = static_cast<BaseRender *>(_relationRender);
        m_relationRender->addRelationRenderChildrenFront_( this );

        this->invalidateColor();

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::removeRelationRender()
    {
        if( m_extraRelationRender != nullptr )
        {
            m_extraRelationRender->removeRelationRenderChildren_( this );
            m_extraRelationRender = nullptr;

            m_relationRender = nullptr;

            return;
        }

        if( m_relationRender == nullptr )
        {
            return;
        }

        m_relationRender->removeRelationRenderChildren_( this );
        m_relationRender = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setExtraRelationRender( RenderInterface * _relationRender )
    {
        MENGINE_ASSERTION_FATAL( _relationRender != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION_FATAL( _relationRender != this, "set this relation" );

        if( m_extraRelationRender != nullptr )
        {
            m_extraRelationRender->removeRelationRenderChildren_( this );
        }
        else if( m_relationRender != nullptr )
        {
            m_relationRender->removeRelationRenderChildren_( this );
        }

        m_extraRelationRender = static_cast<BaseRender *>(_relationRender);
        m_extraRelationRender->addRelationRenderChildrenBack_( this );

        this->invalidateColor();

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::removeExtraRelationRender()
    {
        if( m_extraRelationRender == nullptr )
        {
            return;
        }

        m_extraRelationRender->removeRelationRenderChildren_( this );
        m_extraRelationRender = nullptr;

        if( m_relationRender == nullptr )
        {
            return;
        }

        m_relationRender->addRelationRenderChildrenBack_( this );

        this->invalidateColor();

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseRender::isEmptyRenderChildren() const
    {
        bool result = m_renderChildren.empty();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::clearRenderChildren()
    {
        for( BaseRender * child : m_renderChildren )
        {
            child->m_extraRelationRender = nullptr;
            child->m_relationRender = nullptr;
        }

        m_renderChildren.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::foreachRenderChildren( const LambdaRender & _lambda )
    {
        for( BaseRender * child : m_renderChildren )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::foreachRenderChildrenEnabled( const LambdaRender & _lambda )
    {
        if( m_renderEnable == false )
        {
            return;
        }

        for( BaseRender * child : m_renderChildren )
        {
            if( child->m_renderEnable == false )
            {
                continue;
            }

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderEnable( bool _renderEnable )
    {
        m_renderEnable = _renderEnable;

        this->invalidateRendering();
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

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setHide( bool _hide )
    {
        MENGINE_UNUSED( _hide );

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
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setLocalHide( bool _localHide )
    {
        MENGINE_UNUSED( _localHide );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRelationRenderChildrenBack_( BaseRender * _childRender )
    {
        m_renderChildren.push_back( _childRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::addRelationRenderChildrenFront_( BaseRender * _childRender )
    {
        m_renderChildren.insert( m_renderChildren.begin(), _childRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::removeRelationRenderChildren_( BaseRender * _childRender )
    {
        VectorBaseRender::iterator it_erase = StdAlgorithm::find( m_renderChildren.begin(), m_renderChildren.end(), _childRender );

        MENGINE_ASSERTION_FATAL( it_erase != m_renderChildren.end(), "remove relation child is not found" );

        m_renderChildren.erase( it_erase );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderResolution( const RenderResolutionInterfacePtr & _resolution )
    {
        m_renderResolution = _resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderResolutionInterfacePtr & BaseRender::getRenderResolution() const
    {
        return m_renderResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderViewport( const RenderViewportInterfacePtr & _renderViewport )
    {
        m_renderViewport = _renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & BaseRender::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderCamera( const RenderCameraInterfacePtr & _renderCamera )
    {
        m_renderCamera = _renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & BaseRender::getRenderCamera() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderTransformation( const RenderTransformationInterfacePtr & _renderTransformation )
    {
        m_renderTransformation = _renderTransformation;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & BaseRender::getRenderTransformation() const
    {
        return m_renderTransformation;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderScissor( const RenderScissorInterfacePtr & _renderScissor )
    {
        m_renderScissor = _renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & BaseRender::getRenderScissor() const
    {
        return m_renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setRenderTarget( const RenderTargetInterfacePtr & _renderTarget )
    {
        m_renderTarget = _renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & BaseRender::getRenderTarget() const
    {
        return m_renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setZGroup( ZGroupType _zGroup )
    {
        m_zGroup = _zGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    ZGroupType BaseRender::getZGroup() const
    {
        return m_zGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::setZIndex( ZIndexType _zIndex )
    {
        m_zIndex = _zIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    ZIndexType BaseRender::getZIndex() const
    {
        return m_zIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::mergeRenderContext( const RenderContext * _in, RenderContext * const _out ) const
    {
        _out->resolution = m_renderResolution != nullptr ? m_renderResolution.get() : _in->resolution;
        _out->viewport = m_renderViewport != nullptr ? m_renderViewport.get() : _in->viewport;
        _out->camera = m_renderCamera != nullptr ? m_renderCamera.get() : _in->camera;
        _out->transformation = m_renderTransformation != nullptr ? m_renderTransformation.get() : _in->transformation;
        _out->scissor = m_renderScissor != nullptr ? m_renderScissor.get() : _in->scissor;
        _out->target = m_renderTarget != nullptr ? m_renderTarget.get() : _in->target;
        _out->zGroup = m_zGroup != MENGINE_RENDER_ZGROUP_DEFAULT ? m_zGroup : _in->zGroup;
        _out->zIndex = m_zIndex != MENGINE_RENDER_ZINDEX_DEFAULT ? m_zIndex : _in->zIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::makeRenderContext( RenderContext * const _renderContext ) const
    {
        _renderContext->resolution = m_renderResolution.get();
        _renderContext->viewport = m_renderViewport.get();
        _renderContext->camera = m_renderCamera.get();
        _renderContext->transformation = m_renderTransformation.get();
        _renderContext->scissor = m_renderScissor.get();
        _renderContext->target = m_renderTarget.get();
        _renderContext->zGroup = m_zGroup;
        _renderContext->zIndex = m_zIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::renderWithChildren( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external ) const
    {
        if( this->isRendering() == false )
        {
            return;
        }

        if( m_externalRender == true && _external == false )
        {
            return;
        }

        RenderContext context;
        this->mergeRenderContext( _context, &context );

        if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
        {
            this->render( _renderPipeline, &context );
        }

        for( const BaseRender * child : m_renderChildren )
        {
            child->renderWithChildren( _renderPipeline, &context, false );
        }

        if( context.target != nullptr )
        {
            const RenderInterfacePtr & targetRender = this->makeTargetRender( &context );

            if( targetRender != nullptr )
            {
                targetRender->render( _renderPipeline, _context );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & BaseRender::makeTargetRender( const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _context );

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
        MENGINE_UNUSED( _externalRender );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & BaseRender::getWorldColor() const
    {
        const BaseRender * relationRenderTotal = this->getTotalRelationRender();

        if( relationRenderTotal == nullptr )
        {
            const Color & localColor = Colorable::getLocalColor();

            return localColor;
        }

        if( Colorable::isInvalidateColor() == false )
        {
            const Color & relationColor = Colorable::getRelationColor();

            return relationColor;
        }

        const Color & parentColor = relationRenderTotal->getWorldColor();

        const Color & relationColor = Colorable::updateRelationColor( parentColor );

        return relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::calcTotalColor( Color * const _color ) const
    {
        const Color & worldColor = this->getWorldColor();
        *_color = worldColor;

        const Color & personalColor = this->getPersonalColor();
        *_color *= personalColor;
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
    void BaseRender::invalidateColor() const
    {
        Colorable::invalidateColor();

        for( const BaseRender * child : m_renderChildren )
        {
            child->invalidateColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::_setLocalTransparent( bool _transparent )
    {
        MENGINE_UNUSED( _transparent );

        this->invalidateRendering();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::updateRendering_() const
    {
        m_invalidateRendering = false;

        m_rendering = false;

        if( this->isRenderEnable() == false )
        {
            return;
        }

        if( this->isHide() == true )
        {
            return;
        }

        if( this->isLocalTransparent() == true )
        {
            return;
        }

        m_rendering = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseRender::invalidateRendering()
    {
        m_invalidateRendering = true;
    }
    //////////////////////////////////////////////////////////////////////////
}
