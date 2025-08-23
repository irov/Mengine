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
        , m_arrowRadius( 0.f )
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

        if( m_arrowNode != nullptr )
        {
            m_arrowNode->dispose();
            m_arrowNode = nullptr;
        }

        INPUT_SERVICE()
            ->removeMousePositionProvider( InputMousePositionProviderInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    EArrowType ArrowService::getArrowType() const
    {
        return m_arrowType;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setArrowPoint()
    {
        m_arrowType = EAT_POINT;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setArrowPolygon( const Polygon & _polygon )
    {
        m_arrowType = EAT_POLYGON;

        m_arrowPolygon = _polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & ArrowService::getArrowPolygon() const
    {
        return m_arrowPolygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::setArrowRadius( float _radius )
    {
        m_arrowType = EAT_RADIUS;

        m_arrowRadius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float ArrowService::getArrowRadius() const
    {
        return m_arrowRadius;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & ArrowService::getArrowNode() const
    {
        return m_arrowNode;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
    {
        mt::vec2f adaptScreenPoint;
        Helper::adaptScreenPosition( _screenPoint, &adaptScreenPoint );

        const RenderResolutionInterface * renderResolution = _context->resolution;
        const RenderViewportInterface * renderViewport = _context->viewport;

        const Viewport & viewport = renderViewport->getViewportWM();

        const Resolution & contentResolution = renderResolution->getContentResolution();

        mt::vec2f contentResolutionSizeInv;
        contentResolution.calcSizeInv( &contentResolutionSizeInv );

        mt::vec2f viewportSize;
        viewport.calcSize( &viewportSize );

        mt::vec2f vp_begin = viewport.begin * contentResolutionSizeInv;
        //mt::vec2f vp_end = viewport.end * contentResolutionSizeInv;
        mt::vec2f vp_size = viewportSize * contentResolutionSizeInv;

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
    void ArrowService::calcMouseWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) const
    {
        Helper::screenToWorldDelta( _context, _screenDelta, _worldDelta );
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowService::calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const
    {
        const RenderResolutionInterface * renderResolution = _context->resolution;
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

        mt::vec2f sp_content;
        renderViewport->fromCameraToContentPosition( p_screen, &sp_content );

        mt::vec2f sp_screen;
        renderResolution->fromContentToScreenPosition( sp_content, &sp_screen );

        mt::vec2f adapt_sp;
        Helper::adaptWorldPosition( sp_screen, &adapt_sp );

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

        const NodePtr & node = this->getArrowNode();

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
        NodePtr node = Helper::generateNodeFactorable<Interender>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node, "failed create Interender for arrow" );

        m_arrowNode = node;
    }
    //////////////////////////////////////////////////////////////////////////
}
