#	include "HotSpotPolygon.h"

#   include "HotspotMousePickerVisitor.h"

#	include "Interface/MousePickerSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include "Kernel/Scene.h"
#	include "Layer2D.h"

//#	include "Arrow.h"

#	include "Kernel/ResourceImage.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

#	include "Logger/Logger.h"

namespace Menge
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

		m_polygon.correct();

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
		size_t numPoints = m_polygon.num_points();

		if( numPoints == 0 )
		{
			Node::_updateBoundingBox( _boundingBox );

			return;
		}

		const mt::vec2f * ring = m_polygon.outer_points();

		const mt::mat4f & wm = this->getWorldMatrix();
				
		const mt::vec2f & ring_point_0 = ring[0];

		mt::vec2f wmp_0;
		mt::mul_v2_v2_m4( wmp_0, ring_point_0, wm );

		mt::reset( _boundingBox, wmp_0 );

		for( size_t
			it = 1,
			it_end = numPoints;
		it != it_end;
		++it )
		{
			const mt::vec2f & ring_point_it = ring[it];

			mt::vec2f wmp_it;
			mt::mul_v2_v2_m4( wmp_it, ring_point_it, wm );

			mt::add_internal_point( _boundingBox, wmp_it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotPolygon::testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
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
		this->getPolygonScreen( _camera, _viewport, _contentResolution, &bb, &m_polygonScreen );
		
		if( mt::is_intersect( bb, _point ) == false )
		{
			return m_outward;
		}

		if( m_polygonScreen.intersects( _point ) == false )
		{
			return m_outward;
		}

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotPolygon::testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const
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
		this->getPolygonScreen( _camera, _viewport, _contentResolution, &bb, &m_polygonScreen );

		if( mt::is_intersect( bb, _point, _radius ) == false )
		{
			return m_outward;
		}

		//////TODO: check polygon and circle
		if( m_polygonScreen.intersects( _point ) == false )
		{
			return m_outward;
		}

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotPolygon::testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
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
		this->getPolygonScreen( _camera, _viewport, _contentResolution, &bb_screen, &m_polygonScreen );

		if( mt::is_intersect( bb_screen, bb_polygon ) == false )
		{
			return m_outward;
		}

		if( m_polygonScreen.intersects( m_polygonTemp ) == false )
		{
			return m_outward;
		}

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotPolygon::getPolygonScreen( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, mt::box2f * _bb, Polygon * _screen ) const
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

		size_t numpoints = polygon.num_points();
		const mt::vec2f * points = polygon.outer_points();

		for( size_t it = 0; it != numpoints; ++it )
		{
			const mt::vec2f & v = points[it];

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
	void HotSpotPolygon::_debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask )
	{
		if( (_debugMask & MENGE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		const Polygon & polygon = this->getPolygon();

		size_t numpoints = polygon.num_points();

		if( numpoints == 0 )
		{
			return;
		}

		size_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = _renderService
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		const mt::vec2f * ring = polygon.outer_points();

		for( size_t i = 0; i != numpoints; ++i )
		{
			size_t j = (i + 1) % numpoints;

			mt::vec3f trP0;
			mt::mul_v3_v2_m4( trP0, ring[i], wm );

			RenderVertex2D & v0 = vertices[i * 2 + 0];

			v0.position = trP0;

			v0.color = m_debugColor;

			for( size_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;
			}

			mt::vec3f trP1;
			mt::mul_v3_v2_m4( trP1, ring[j], wm );

			RenderVertex2D & v1 = vertices[i * 2 + 1];

			v1.position = trP1;

			v1.color = m_debugColor;

			for( size_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDebugMaterial();

		_renderService
			->addRenderLine( _state, debugMaterial
			, vertices
			, (uint32_t)vertexCount
			, nullptr
			, true
			);
	}
}
