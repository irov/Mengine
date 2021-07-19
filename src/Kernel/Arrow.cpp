#include "Arrow.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/RenderInterface.h"

#include "ArrowInputMousePositionProvider.h"

#include "Kernel/Entity.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Arrow::Arrow()
        : m_arrowType( EAT_POINT )
        , m_inputMousePositionProviderId( INVALID_UNIQUE_ID )
        , m_clickPoint( 0.f, 0.f )
        , m_clickRadius( 0.f )
        , m_hided( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Arrow::~Arrow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Arrow::initialize()
    {
        ArrowInputMousePositionProviderPtr provider = Helper::makeFactorableUnique<ArrowInputMousePositionProvider>( MENGINE_DOCUMENT_FACTORABLE, this );

        UniqueId inputMousePositionProviderId = INPUT_SERVICE()
            ->addMousePositionProvider( provider, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_UNIQUE_INVALID( inputMousePositionProviderId );

        m_inputMousePositionProviderId = inputMousePositionProviderId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::finalize()
    {
        INPUT_SERVICE()
            ->removeMousePositionProvider( m_inputMousePositionProviderId );

        m_inputMousePositionProviderId = INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setArrowType( EArrowType _arrowType )
    {
        m_arrowType = _arrowType;
    }
    //////////////////////////////////////////////////////////////////////////
    EArrowType Arrow::getArrowType() const
    {
        return m_arrowType;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setEntity( const EntityPtr & _entity )
    {
        m_entity = _entity;
    }
    //////////////////////////////////////////////////////////////////////////
    const EntityPtr & Arrow::getEntity() const
    {
        return m_entity;
    }
    ////////////////////////////////////////////////////////////////////////////
    //bool Arrow::_activate()
    //{


    //    if( m_entity != nullptr )
    //    {
    //        const RenderInterface * render = m_entity->getRender();

    //        RenderContext context;
    //        render->makeRenderContext( &context );

    //        if( context.camera != nullptr && context.viewport != nullptr )
    //        {
    //            const mt::vec2f & cursor_pos = INPUT_SERVICE()
    //                ->getCursorPosition( TC_TOUCH0 );

    //            mt::vec2f wp;
    //            this->calcMouseWorldPosition( &context, cursor_pos, &wp );

    //            mt::vec3f pos;
    //            pos.x = wp.x;
    //            pos.y = wp.y;
    //            pos.z = 0.f;

    //            TransformationInterface * transformation = m_entity->getTransformation();
    //            transformation->setLocalPosition( pos );
    //        }
    //        else
    //        {
    //            mt::vec3f pos;
    //            pos.x = 0.f;
    //            pos.y = 0.f;
    //            pos.z = 0.f;

    //            TransformationInterface * transformation = m_entity->getTransformation();
    //            transformation->setLocalPosition( pos );
    //        }
    //    }

    //    return true;
    //}
    ////////////////////////////////////////////////////////////////////////////
    //void Arrow::_deactivate()
    //{

    //}
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setClickPoint( const mt::vec2f & _clickPoint )
    {
        m_clickPoint = _clickPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Arrow::getClickPoint() const
    {
        return m_clickPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setClickPolygon( const Polygon & _clickPolygon )
    {
        m_clickPolygon = _clickPolygon;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & Arrow::getClickPolygon() const
    {
        return m_clickPolygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setClickRadius( float _clickRadius )
    {
        m_clickRadius = _clickRadius;
    }
    //////////////////////////////////////////////////////////////////////////
    float Arrow::getClickRadius() const
    {
        return m_clickRadius;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::onAppMouseLeave()
    {
        RenderInterface * render = m_entity->getRender();

        render->setHide( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::onAppMouseEnter()
    {
        if( m_hided == false )
        {
            RenderInterface * render = m_entity->getRender();

            render->setHide( false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const
    {
        mt::vec2f adaptScreenPoint;
        this->adaptScreenPosition( _screenPoint, &adaptScreenPoint );

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
                const mt::vec2f & pc = this->getClickPoint();

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

        mt::vec2f p_screen = (p_vm_pm + mt::vec2f( 1.f, 1.f )) * 0.5f;

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
        this->adaptWorldPosition( sp, &adapt_sp );

        *_screenPoint = adapt_sp;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const
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
    void Arrow::adaptScreenDeltha( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha ) const
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
    void Arrow::adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const
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
