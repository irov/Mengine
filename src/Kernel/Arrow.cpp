#include "Arrow.h"

#include "Interface/InputSystemInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/RenderCameraHelper.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Arrow::ArrowInputMousePositionProvider
        : public FactorableUnique<Factorable>
        , public InputMousePositionProviderInterface
    {
    public:
        ArrowInputMousePositionProvider( Arrow * _arrow )
            : m_arrow( _arrow )
        {
        }

        ~ArrowInputMousePositionProvider() override
        {
        }
        
    protected:
        void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position ) override
        {
            if( _touchId != 0 )
            {
                return;
            }

            const RenderCameraInterfacePtr & renderCamera = m_arrow->getRenderCamera();
            const RenderViewportInterfacePtr & renderViewport = m_arrow->getRenderViewport();

            mt::vec2f wp;
            m_arrow->calcMouseWorldPosition( renderCamera, renderViewport, _position, wp );

            mt::vec3f v3( wp.x, wp.y, 0.f );

            m_arrow->setLocalPosition( v3 );
        }

    protected:
        Arrow * m_arrow;
    };
    //////////////////////////////////////////////////////////////////////////
    Arrow::Arrow()
        : m_arrowType( EAT_POINT )
        , m_inputMousePositionProviderId( 0 )
        , m_pointClick( 0.f, 0.f )
        , m_radius( 0.f )
        , m_hided( false )
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

        m_inputMousePositionProviderId = INPUT_SERVICE()
            ->addMousePositionProvider( new ArrowInputMousePositionProvider( this ) );

        const mt::vec2f & cursor_pos = INPUT_SERVICE()
            ->getCursorPosition( 0 );

        const RenderCameraInterfacePtr & renderCamera = this->getRenderCamera();
        const RenderViewportInterfacePtr & renderViewport = this->getRenderViewport();

        if( renderCamera != nullptr && renderViewport != nullptr )
        {
            mt::vec2f wp;
            this->calcMouseWorldPosition( renderCamera, renderViewport, cursor_pos, wp );

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

        Entity::_deactivate();
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
        Node::setHide( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::onAppMouseEnter()
    {
        if( m_hided == false )
        {
            Node::setHide( false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::setHide( bool _value )
    {
        Node::setHide( _value );
        m_hided = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcMouseWorldPosition( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const
    {
        mt::vec2f adaptScreenPoint;
        this->adaptScreenPosition_( _screenPoint, adaptScreenPoint );

        const Viewport & viewport = _viewport->getViewport();

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = (adaptScreenPoint - vp_begin) / vp_size;

        const mt::mat4f & pm_inv = _camera->getCameraProjectionMatrixInv();

        mt::vec2f p1 = sp * 2.f - mt::vec2f( 1.f, 1.f );
        p1.y = -p1.y;

        mt::vec2f p_pm;
        mt::mul_v2_v2_m4( p_pm, p1, pm_inv );

        const mt::mat4f & vm_inv = _camera->getCameraViewMatrixInv();

        mt::vec2f p = p_pm;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( p_vm, p, vm_inv );

        _worldPoint = p_vm;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcPointClick( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const
    {
        (void)_viewport;
        (void)_camera;

        mt::vec2f p1;
        this->calcMouseWorldPosition( _camera, _viewport, _screenPoint, p1 );

        EArrowType arrowType = this->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                const mt::vec2f & pc = this->getOffsetClick();

                mt::vec2f p = p1 + pc;

                _worldPoint = p;
            }break;
        case EAT_RADIUS:
            {
                mt::vec2f p = p1;

                _worldPoint = p;
            }break;
        case EAT_POLYGON:
            {
                mt::vec2f p = p1;

                _worldPoint = p;
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcPointDeltha( const RenderCameraInterfacePtr & _camera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) const
    {
        Helper::screenToWorldDelta( _camera, _screenPoint, _screenDeltha, _worldDeltha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::calcMouseScreenPosition( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _worldPoint, mt::vec2f & _screenPoint ) const
    {
        const mt::mat4f & vm = _camera->getCameraViewMatrix();

        mt::vec2f p = _worldPoint;

        mt::vec2f p_vm;
        mt::mul_v2_v2_m4( p_vm, p, vm );

        const Viewport & viewport = _viewport->getViewport();
        p_vm += viewport.begin;

        const mt::mat4f & pm = _camera->getCameraProjectionMatrix();

        mt::vec2f p_vm_pm;
        mt::mul_v2_v2_m4( p_vm_pm, p_vm, pm );

        p_vm_pm.y = -p_vm_pm.y;

        mt::vec2f p_screen = (p_vm_pm + mt::vec2f( 1.f, 1.f )) / 2.f;

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( contentResolutionSize );

        mt::vec2f viewportSize;
        viewport.calcSize( viewportSize );

        mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
        //mt::vec2f vp_end = viewport.end / contentResolutionSize;
        mt::vec2f vp_size = viewportSize / contentResolutionSize;

        mt::vec2f sp = vp_begin + p_screen * vp_size;

        mt::vec2f adapt_sp;
        this->adaptWorldPosition_( sp, adapt_sp );

        _screenPoint = adapt_sp;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptScreenPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
        mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

        _adaptScreenPoint = (_screenPoint - windowOffset) / windowScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptScreenDeltha_( const mt::vec2f & _screenDeltha, mt::vec2f & _adaptScreenDeltha ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;

        _adaptScreenDeltha = _screenDeltha / windowScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::adaptWorldPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const
    {
        const Viewport & renderViewport = APPLICATION_SERVICE()
            ->getRenderViewport();

        const Resolution & currentResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        mt::vec2f renderViewportSize;
        renderViewport.calcSize( renderViewportSize );

        mt::vec2f currentResolutionSize;
        currentResolution.calcSize( currentResolutionSize );

        mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
        mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

        _adaptScreenPoint = _screenPoint * windowScale + windowOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::_debugRender( const RenderContext * _state )
    {
        if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
        {
            return;
        }

        EArrowType arrowType = this->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                return;
            }break;
        case EAT_RADIUS:
            {
                uint32_t numpoints = 4;
                uint32_t vertexCount = numpoints * 2;

                RenderVertex2D * vertices = this->getDebugRenderVertex2D( vertexCount );

                if( vertices == nullptr )
                {
                    return;
                }

                const mt::mat4f & worldMat = this->getWorldMatrix();

                mt::vec2f ring[4];

                float radius = this->getRadius();
                float half_radius = radius * 0.5f;
                ring[0] = mt::vec2f( 0, -half_radius );
                ring[1] = mt::vec2f( half_radius, 0 );
                ring[2] = mt::vec2f( 0, half_radius );
                ring[3] = mt::vec2f( -half_radius, 0 );

                for( uint32_t i = 0; i != numpoints; ++i )
                {
                    uint32_t j = (i + 1) % numpoints;

                    mt::vec3f trP0;
                    mt::mul_v3_v2_m4( trP0, ring[i], worldMat );

                    RenderVertex2D & v0 = vertices[i * 2 + 0];

                    v0.position = trP0;

                    v0.color = 0x8080FFFF;

                    for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
                    {
                        v0.uv[uv_index].x = 0.f;
                        v0.uv[uv_index].y = 0.f;
                    }

                    mt::vec3f trP1;
                    mt::mul_v3_v2_m4( trP1, ring[j], worldMat );

                    RenderVertex2D & v1 = vertices[i * 2 + 1];

                    v1.position = trP1;

                    v1.color = 0x8080FFFF;

                    for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
                    {
                        v1.uv[uv_index].x = 0.f;
                        v1.uv[uv_index].y = 0.f;
                    }
                }

                const RenderMaterialInterfacePtr & debugMaterial = this->getDebugMaterial();

                this->addRenderLine( _state, debugMaterial
                        , vertices
                        , vertexCount
                        , nullptr
                        , true
                    );
            }break;
        case EAT_POLYGON:
            {
                uint32_t numpoints = m_polygon.num_points();

                if( numpoints == 0 )
                {
                    return;
                }

                uint32_t vertexCount = numpoints * 2;

                RenderVertex2D * vertices = this->getDebugRenderVertex2D( vertexCount );

                if( vertices == nullptr )
                {
                    return;
                }

                const mt::mat4f & worldMat = this->getWorldMatrix();

                const mt::vec2f * ring = m_polygon.outer_points();

                for( uint32_t i = 0; i != numpoints; ++i )
                {
                    uint32_t j = (i + 1) % numpoints;

                    mt::vec3f trP0;
                    mt::mul_v3_v2_m4( trP0, ring[i], worldMat );

                    RenderVertex2D & v0 = vertices[i * 2 + 0];

                    v0.position = trP0;

                    v0.color = 0x8080FFFF;

                    for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
                    {
                        v0.uv[uv_index].x = 0.f;
                        v0.uv[uv_index].y = 0.f;
                    }

                    mt::vec3f trP1;
                    mt::mul_v3_v2_m4( trP1, ring[j], worldMat );

                    RenderVertex2D & v1 = vertices[i * 2 + 1];

                    v1.position = trP1;

                    v1.color = 0x8080FFFF;

                    for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
                    {
                        v1.uv[uv_index].x = 0.f;
                        v1.uv[uv_index].y = 0.f;
                    }
                }

                const RenderMaterialInterfacePtr & debugMaterial = this->getDebugMaterial();

                this->addRenderLine( _state, debugMaterial
                        , vertices
                        , (uint32_t)vertexCount
                        , nullptr
                        , true
                    );
            }break;
        }
    }
}
