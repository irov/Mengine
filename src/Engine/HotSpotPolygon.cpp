#include "HotSpotPolygon.h"

#include "HotspotMousePickerVisitor.h"

#include "Interface/MousePickerSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderViewportInterface.h"

#include "Kernel/PolygonHelper.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"

#include "pybind/system.hpp"
#include "pybind/extract.hpp"

#include "stdex/span.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygon::HotSpotPolygon()
        : m_invalidatePolygonWM( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygon::~HotSpotPolygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::setPolygon( const Polygon & _polygon )
    {
        m_polygon = _polygon;

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & HotSpotPolygon::getPolygon() const
    {
        return m_polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::clearPoints()
    {
        m_polygon.clear();

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::_updateBoundingBox( mt::box2f & _boundingBox ) const
    {
        const VectorPoints & points = m_polygon.getPoints();

        if( points.empty() == true )
        {
            Node::_updateBoundingBox( _boundingBox );

            return;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::vec2f & v0 = points.front();

        mt::vec2f wmp_0;
        mt::mul_v2_v2_m4( wmp_0, v0, wm );

        mt::reset( _boundingBox, wmp_0 );

        for( const mt::vec2f & v : stdex::span::range( points, 1 ) )
        {
            mt::vec2f wmp_it;
            mt::mul_v2_v2_m4( wmp_it, v, wm );

            mt::add_internal_point( _boundingBox, wmp_it );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        mt::box2f bb;
        this->getScreenPolygon( _camera, _viewport, _contentResolution, &bb, &m_polygonScreen );

        if( mt::is_intersect( bb, _point ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, _point ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        mt::box2f bb;
        this->getScreenPolygon( _camera, _viewport, _contentResolution, &bb, &m_polygonScreen );

        if( mt::is_intersect( bb, _point, _radiusx, _radiusy ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, _point, _radiusx, _radiusy ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        if( _polygon.empty() == true )
        {
            return m_outward;
        }

        _polygon.transpose( m_polygonTemp, _point );

        mt::box2f bb_polygon;
        m_polygonTemp.to_box2f( bb_polygon );

        mt::box2f bb_screen;
        this->getScreenPolygon( _camera, _viewport, _contentResolution, &bb_screen, &m_polygonScreen );

        if( mt::is_intersect( bb_screen, bb_polygon ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, m_polygonTemp ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::getScreenPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, mt::box2f * _bb, Polygon * _screen ) const
    {
        if( _bb != nullptr )
        {
            mt::ident_box( *_bb );
        }

        if( _screen != nullptr )
        {
            _screen->clear();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, wm, vpm );

        const Viewport & vp = _viewport->getViewport();

        mt::vec2f vp_size;
        vp.calcSize( vp_size );

        mt::vec2f contentResolutionInvSize;
        _contentResolution.calcInvSize( contentResolutionInvSize );

        const Polygon & polygon = this->getPolygon();

        const VectorPoints & points = polygon.getPoints();

        for( const mt::vec2f & v : points )
        {
            mt::vec2f v_wvp;
            mt::mul_v2_v2_m4_homogenize( v_wvp, v, wvpm );

            mt::vec2f v_wvpn;
            v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
            v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

            mt::vec2f v_screen = (vp.begin + v_wvpn * vp_size) * contentResolutionInvSize;

            if( _bb != nullptr )
            {
                mt::add_internal_point( *_bb, v_screen );
            }

            if( _screen != nullptr )
            {
                _screen->append( v_screen );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::_debugRender( const RenderContext * _state )
    {
        if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
        {
            return;
        }

        if( m_debugColor == 0x00000000 )
        {
            return;
        }

        const Polygon & polygon = this->getPolygon();

        uint32_t numpoints = polygon.size();

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

        const mt::mat4f & wm = this->getWorldMatrix();

        const VectorPoints & ring = polygon.getPoints();

        for( uint32_t i = 0; i != numpoints; ++i )
        {
            uint32_t j = (i + 1) % numpoints;

            mt::vec3f trP0;
            mt::mul_v3_v2_m4( trP0, ring[i], wm );

            RenderVertex2D & v0 = vertices[i * 2 + 0];

            v0.position = trP0;

            v0.color = m_debugColor;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v0.uv[uv_index].x = 0.f;
                v0.uv[uv_index].y = 0.f;
            }

            mt::vec3f trP1;
            mt::mul_v3_v2_m4( trP1, ring[j], wm );

            RenderVertex2D & v1 = vertices[i * 2 + 1];

            v1.position = trP1;

            v1.color = m_debugColor;

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
    }
}
