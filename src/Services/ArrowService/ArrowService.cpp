#include "ArrowService.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionUnique.h"
#include "Kernel/ResolutionHelper.h"
#include "Kernel/Interender.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ArrowService, Mengine::ArrowService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArrowService::ArrowService()
        : m_arrowType( EAT_POINT )
        , m_pointClick( 0.f, 0.f )
        , m_radius( 0.f )
        , m_hided( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowService::~ArrowService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArrowService::_initializeService()
    {
        INPUT_SERVICE()
            ->addMousePositionProvider( InputMousePositionProviderInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_COMPLETE, &ArrowService::notifyInitializeComplete_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_COMPLETE );

        m_node = nullptr;

        INPUT_SERVICE()
            ->removeMousePositionProvider( InputMousePositionProviderInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    EArrowType ArrowService::getArrowType() const
    {
        return m_arrowType;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setOffsetClick( const mt::vec2f & _offsetClick )
    {
        m_arrowType = EAT_POINT;

        m_pointClick = _offsetClick;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ArrowService::getOffsetClick() const
    {
        return m_pointClick;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setPolygon( const Polygon & _polygon )
    {
        m_arrowType = EAT_POLYGON;

        m_polygon = _polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & ArrowService::getPolygon() const
    {
        return m_polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setRadius( float _radius )
    {
        m_arrowType = EAT_RADIUS;

        m_radius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float ArrowService::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & ArrowService::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
    {
        mt::vec2f adaptScreenPoint;
        Helper::adaptScreenPosition( _screenPoint, &adaptScreenPoint );

        const RenderViewportInterface * renderViewport = _context->viewport;

        const Viewport & viewport = renderViewport->getViewportWM();

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( &viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = (adaptScreenPoint - vp_begin) / vp_size;

        const RenderCameraInterface * renderCamera = _context->camera;

        const mt::mat4f & pm_inv = renderCamera->getCameraProjectionMatrixInv();

        mt::vec2f p1 = sp * 2.f - mt::vec2f( 1.f, 1.f );
        p1.y = -p1.y;

        mt::vec2f p_pm;
        mt::mul_v2_v2_m4( &p_pm, p1, pm_inv );

        const mt::mat4f & vm_inv = renderCamera->getCameraViewMatrixInv();

        mt::vec2f p = p_pm;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( &p_vm, p, vm_inv );

        *_worldPoint = p_vm;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
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
    void ArrowService::calcPointDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) const
    {
        Helper::screenToWorldDelta( _context, _screenDelta, _worldDelta );
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const
    {
        const RenderCameraInterface * renderCamera = _context->camera;

        const mt::mat4f & vm = renderCamera->getCameraViewMatrix();

        mt::vec2f p = _worldPoint;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( &p_vm, p, vm );

        const RenderViewportInterface * renderViewport = _context->viewport;

        const Viewport & viewport = renderViewport->getViewportWM();
        p_vm += viewport.begin;

        const mt::mat4f & pm = renderCamera->getCameraProjectionMatrix();

        mt::vec2f p_vm_pm;
        mt::mul_v2_v2_m4( &p_vm_pm, p_vm, pm );

        p_vm_pm.y = -p_vm_pm.y;

        mt::vec2f p_screen = (p_vm_pm + mt::vec2f( 1.f, 1.f )) / 2.f;

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( &viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = vp_begin + p_screen * vp_size;

        mt::vec2f adapt_sp;
        Helper::adaptWorldPosition( sp, &adapt_sp );

        *_screenPoint = adapt_sp;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure )
    {
        MENGINE_UNUSED( _pressure );

        if( _touchId != TC_TOUCH0 )
        {
            return;
        }

        const NodePtr & node = this->getNode();

        if( node == nullptr )
        {
            return;
        }

        RenderInterface * render = node->getRender();

        RenderContext context;
        render->makeRenderContext( &context );

        mt::vec2f wp;
        this->calcMouseWorldPosition( &context, _position, &wp );

        mt::vec3f v3( wp.x, wp.y, 0.f );

        TransformationInterface * transformation = node->getTransformation();

        transformation->setLocalPosition( v3 );
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::notifyInitializeComplete_()
    {
        NodePtr node = Helper::generateFactorable<Node, Interender>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node, "failed create Interender for arrow" );

        m_node = node;
    }
    //////////////////////////////////////////////////////////////////////////
}
