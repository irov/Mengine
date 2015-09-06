#	include "HotSpotPolygon.h"

#   include "HotspotMousePickerVisitor.h"

#	include "Interface/MousePickerSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Player.h"

#	include "Kernel/Scene.h"
#	include "Layer2D.h"

#	include "Arrow.h"

#	include "Kernel/ResourceImage.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotPolygon::HotSpotPolygon()
		: m_invalidatePolygonWM(true)
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

		boost::geometry::correct( m_polygon );

		this->invalidateBoundingBox();
		this->invalidatePolygonWM();
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
	void HotSpotPolygon::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidatePolygonWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotPolygon::invalidatePolygonWM()
	{
		m_invalidatePolygonWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotPolygon::updatePolygonWM_() const
	{
		m_invalidatePolygonWM = false;

		if( this->isCompile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("HotSpotPolygon::updatePolygonWM_ %s not compile"
				, this->getName().c_str()
				);

			return;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		polygon_wm( m_polygonWM, m_polygon, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotPolygon::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		uint32_t numPoints = boost::geometry::num_points( m_polygon );

		if( numPoints == 0 )
		{
			Node::_updateBoundingBox( _boundingBox );

			return;
		}

		const Polygon::ring_type & ring = m_polygon.outer();

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec2f wmp;
		mt::mul_v2_m4( wmp, ring[0], wm );

		mt::reset( _boundingBox, wmp );

		for( uint32_t
			it = 1,
			it_end = numPoints;
		it != it_end; 
		++it )
		{
			mt::vec2f wmp;
			mt::mul_v2_m4( wmp, ring[it], wm );

			mt::add_internal_point( _boundingBox, wmp );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPoint( const mt::vec2f & _point ) const
    {
		if( m_global == true )
		{
			return !m_outward;
		}

		if( polygon_empty( m_polygon ) == true )
		{
			return m_outward;
		}

		const mt::box2f & bb = this->getBoundingBox();

		if( mt::is_intersect( bb, _point ) == false )
		{
			return m_outward;
		}

		const Polygon & polygonWM = this->getPolygonWM();

		GeometryPoint p(_point.x, _point.y);

		if( boost::geometry::intersects( polygonWM, p ) == false )
		{
			return m_outward;
		}

		return !m_outward;
    }
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotPolygon::testRadius( const mt::vec2f & _point, float _radius ) const
	{
		if( m_global == true )
		{
			return !m_outward;
		}

		if( polygon_empty( m_polygon ) == true )
		{
			return m_outward;
		}

		const mt::box2f & bb = this->getBoundingBox();

		if( mt::is_intersect( bb, _point, _radius ) == false )
		{
			return m_outward;
		}

		const Polygon & polygonWM = this->getPolygonWM();

		GeometryPoint p(_point.x, _point.y);

		//TODO: check polygon and circle
		if( boost::geometry::intersects( polygonWM, p ) == false )
		{
			return m_outward;
		}

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotPolygon::testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const
	{
		if( m_global == true )
		{
			return !m_outward;
		}

		if( polygon_empty( m_polygon ) == true )
		{
			return m_outward;
		}

		if( polygon_empty( _polygon ) == true )
		{
			return m_outward;
		}

		const mt::box2f & bb = this->getBoundingBox();

		polygon_transpose( m_polygonTemp, _polygon, _point );
		
		mt::box2f bb_screen;
		if( polygon_to_box2f( bb_screen, m_polygonTemp ) == false )
		{
			return m_outward;
		}

		if( mt::is_intersect( bb, bb_screen ) == false )
		{
			return m_outward;
		}

		const Polygon & polygonWM = this->getPolygonWM();

		if( boost::geometry::intersects( polygonWM, m_polygonTemp ) == false )
		{
			return m_outward;
		}

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotPolygon::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		uint32_t numpoints = boost::geometry::num_points(m_polygon);

		if( numpoints == 0 )
		{
			return;
		}

		uint32_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		const mt::mat4f & worldMat = this->getWorldMatrix();

		const Polygon::ring_type & ring = m_polygon.outer();

		for( uint32_t i = 0; i != numpoints; ++i )
		{
			uint32_t j = (i + 1) % numpoints;

			mt::vec2f trP0;
			mt::mul_v2_m4( trP0, ring[i], worldMat );

			RenderVertex2D & v0 = vertices[i*2+0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = m_debugColor;
			v0.uv[0].x = 0.f;
			v0.uv[0].y = 0.f;
			v0.uv[1].x = 0.f;
			v0.uv[1].y = 0.f;

			mt::vec2f trP1;
			mt::mul_v2_m4( trP1, ring[j], worldMat );

			RenderVertex2D & v1 = vertices[i*2+1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = m_debugColor;
			v1.uv[0].x = 0.f;
			v1.uv[0].y = 0.f;
			v1.uv[1].x = 0.f;
			v1.uv[1].y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
            , vertices
			, vertexCount
			, nullptr
			, true
            );
	}
}
