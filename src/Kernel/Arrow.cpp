#include "Arrow.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "ArrowInputMousePositionProvider.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Arrow::Arrow()
        : m_arrowType( EAT_POINT )
        , m_inputMousePositionProviderId( INVALIDATE_UNIQUE_ID )
        , m_pointClick( 0.f, 0.f )
        , m_radius( 0.f )
        , m_hided( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Arrow::~Arrow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EArrowType Arrow::getArrowType() const
    {
        return m_arrowType;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
    {
        m_arrowType = EAT_POINT;

        m_pointClick = _offsetClick;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Arrow::getOffsetClick() const
    {
        return m_pointClick;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Arrow::_activate()
    {
        if( Entity::_activate() == false )
        {
            return false;
        }

        ArrowInputMousePositionProviderPtr provider = Helper::makeFactorableUnique<ArrowInputMousePositionProvider>( MENGINE_DOCUMENT_FACTORABLE, this );

        UniqueId inputMousePositionProviderId = INPUT_SERVICE()
            ->addMousePositionProvider( provider, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_UNIQUE_INVALID( inputMousePositionProviderId );

        m_inputMousePositionProviderId = inputMousePositionProviderId;

        const mt::vec2f & cursor_pos = INPUT_SERVICE()
            ->getCursorPosition( TC_TOUCH0 );

        RenderContext context;
        this->makeRenderContext( &context );

        if( context.camera != nullptr && context.viewport != nullptr )
        {
            mt::vec2f wp;
            this->calcMouseWorldPosition( &context, cursor_pos, &wp );

            mt::vec3f pos;
            pos.x = wp.x;
            pos.y = wp.y;
            pos.z = 0.f;

            this->setLocalPosition( pos );
        }
        else
        {
            mt::vec3f pos;
            pos.x = 0.f;
            pos.y = 0.f;
            pos.z = 0.f;

            this->setLocalPosition( pos );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::_deactivate()
    {
        INPUT_SERVICE()
            ->removeMousePositionProvider( m_inputMousePositionProviderId );
        m_inputMousePositionProviderId = 0;

        Entity::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setPolygon( const Polygon & _polygon )
    {
        m_arrowType = EAT_POLYGON;

        m_polygon = _polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & Arrow::getPolygon() const
    {
        return m_polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setRadius( float _radius )
    {
        m_arrowType = EAT_RADIUS;

        m_radius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float Arrow::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::onAppMouseLeave()
    {
        BaseRender::setHide( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::onAppMouseEnter()
    {
        if( m_hided == false )
        {
            BaseRender::setHide( false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
    {
        mt::vec2f adaptScreenPoint;
        this->adaptScreenPosition_( _screenPoint, &adaptScreenPoint );

        const RenderViewportInterface * renderViewport = _context->viewport;

        const Viewport & viewport = renderViewport->getViewport();

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = (adaptScreenPoint - vp_begin) / vp_size;

        const RenderCameraInterface * renderCamera = _context->camera;

        const mt::mat4f & pm_inv = renderCamera->getCameraProjectionMatrixInv();

        mt::vec2f p1 = sp * 2.f - mt::vec2f( 1.f, 1.f );
        p1.y = -p1.y;

        mt::vec2f p_pm;
        mt::mul_v2_v2_m4( p_pm, p1, pm_inv );

        const mt::mat4f & vm_inv = renderCamera->getCameraViewMatrixInv();

        mt::vec2f p = p_pm;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( p_vm, p, vm_inv );

        *_worldPoint = p_vm;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
    {
        MENGINE_UNUSED( _context );

        mt::vec2f p1;
        this->calcMouseWorldPosition( _context, _screenPoint, &p1 );

        EArrowType arrowType = this->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                const mt::vec2f & pc = this->getOffsetClick();

                mt::vec2f p = p1 + pc;

                *_worldPoint = p;
            }break;
        case EAT_RADIUS:
            {
                mt::vec2f p = p1;

                *_worldPoint = p;
            }break;
        case EAT_POLYGON:
            {
                mt::vec2f p = p1;

                *_worldPoint = p;
            }break;
        default:
            {
                MENGINE_ASSERTION_FATAL( false, "arrow type '%u'"
                    , arrowType
                );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcPointDeltha( const RenderContext * _context, const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha ) const
    {
        Helper::screenToWorldDelta( _context, _screenDeltha, _worldDeltha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const
    {
        const RenderCameraInterface * renderCamera = _context->camera;

        const mt::mat4f & vm = renderCamera->getCameraViewMatrix();

        mt::vec2f p = _worldPoint;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( p_vm, p, vm );

        const RenderViewportInterface * renderViewport = _context->viewport;

        const Viewport & viewport = renderViewport->getViewport();
        p_vm += viewport.begin;

        const mt::mat4f & pm = renderCamera->getCameraProjectionMatrix();

        mt::vec2f p_vm_pm;
        mt::mul_v2_v2_m4( p_vm_pm, p_vm, pm );

        p_vm_pm.y = -p_vm_pm.y;

        mt::vec2f p_screen = (p_vm_pm + mt::vec2f( 1.f, 1.f )) / 2.f;

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = vp_begin + p_screen * vp_size;

        mt::vec2f adapt_sp;
        this->adaptWorldPosition_( sp, &adapt_sp );

        *_screenPoint = adapt_sp;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptScreenPosition_( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( &currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
        mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

        *_adaptScreenPoint = (_screenPoint - windowOffset) / windowScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptScreenDeltha_( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( &currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;

        *_adaptScreenDeltha = _screenDeltha / windowScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptWorldPosition_( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( &currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
        mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

        *_adaptScreenPoint = _screenPoint * windowScale + windowOffset;
    }
    //////////////////////////////////////////////////////////////////////////
}
