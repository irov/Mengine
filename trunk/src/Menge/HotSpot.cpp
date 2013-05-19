#	include "HotSpot.h"

#	include "Interface/MousePickerSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Kernel/Scene.h"
#	include "Layer2D.h"

#	include "Arrow.h"

#	include "Interface/ResourceInterface.h"
#	include "ResourceImage.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
		: m_debugColor(0x80FFFFFF)
	{
        m_mousePickerAdapter.setHotspot( this );
        m_mousePickerAdapter.setDefaultHandle( true );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_mousePickerAdapter.setServiceProvider( _serviceProvider );
    }
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setPolygon( const Polygon & _polygon )
	{
		m_polygon = _polygon;

		boost::geometry::correct( m_polygon );

		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & HotSpot::getPolygon() const
	{
		return m_polygon;
	}
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setDefaultHandle( bool _handle )
    {
        m_mousePickerAdapter.setDefaultHandle( _handle );
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getDefaultHandle() const
    {
        bool handle = m_mousePickerAdapter.getDefaultHandle();

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setDebugColor( uint32 _color )
    {
        m_debugColor = _color;

        VectorVertices::invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32 HotSpot::getDebugColor() const
    {
        return m_debugColor;
    }
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint_( const mt::vec2f & _p )
	{
		boost::geometry::append( m_polygon, _p );

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		m_mousePickerAdapter.setEventListener( _listener );

		this->registerEvent( EVENT_ACTIVATE, ("onActivate"), _listener );
		this->registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

        m_mousePickerAdapter.activatePicker();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACTIVATE)( "()" );

//#	ifndef MENGE_MASTER_RELEASE
		if( m_enable )
		{
			m_debugColor = 0xA0FFFFFF;
		}
		else
		{
			m_debugColor = 0x20FFFFFF;
		}

		VectorVertices::invalidateVertices();
//#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		m_mousePickerAdapter.deactivatePicker();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)( "()" );

//#	ifndef MENGE_MASTER_RELEASE
		m_debugColor = 0x00000000;
		VectorVertices::invalidateVertices();
//#	endif
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		RenderCameraInterface * camera = PLAYER_SERVICE(m_serviceProvider)
			->getCamera2D();

		const Viewport & viewport = camera->getViewport();

		//const mt::vec2f & direction = this->getWorldDirection();
		//const mt::vec2f & position = this->getScreenPosition();

		mt::box2f myBBox;
		this->getBoundingBox( myBBox );

		if( m_layer != 0 )
		{
			if( m_layer->testBoundingBox( viewport, myBBox, mt::box2f( _p, _p ) ) == false )
			{
				return false;
			}

			if( m_layer->testPoint( camera, this, _p ) == false )
			{
				return false;
			}
		}
		else
		{
			if( mt::is_intersect( myBBox, mt::box2f( _p, _p ) ) == false )
			{
				return false;
			}

			const mt::mat4f & wm = getWorldMatrix();
			
			mt::vec2f wmp;
			mt::mul_v2_m4( wmp, _p, wm );

			Polygon point_polygon;
			boost::geometry::append(point_polygon, wmp);

			//bool result = mt::is_point_inside_polygon_wm( m_polygon, _p, wm );
			bool result = boost::geometry::intersects( m_polygon, point_polygon );

			return result;
		}

		//bool result = mt::is_point_inside_polygon(m_polygon, _p, position, direction);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		size_t numPoints = boost::geometry::num_points( m_polygon );

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

		for( size_t
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
	void HotSpot::_update( float _current, float _timing )
	{
		Node::_update( _current, _timing );

		m_mousePickerAdapter.updatePicker();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::mat4f& _transform, const Polygon & _screenPoly, const mt::mat4f& _screenTransform )
	{
		bool intersect = false;

		m_polygonWM.clear();
		polygon_wm( m_polygonWM, m_polygon, _transform );

        mt::box2f bb;
        if( polygon_to_box2f( bb, m_polygonWM ) == false )
        {
            return false;
        }

		size_t num_of_screen_poly_points = boost::geometry::num_points(_screenPoly);

		if( num_of_screen_poly_points == 1 )
		{
			const Polygon::ring_type & ring = _screenPoly.outer();

			mt::vec2f wmp;
			mt::mul_v2_m4( wmp, ring[0], _screenTransform );

            if( mt::is_intersect( bb, wmp ) == false )
            {
                return false;
            }

			m_polygonScreen.clear();
			boost::geometry::append(m_polygonScreen, wmp);
			
			intersect = boost::geometry::intersects( m_polygonWM, m_polygonScreen );
		}
		else
		{
			m_polygonScreen.clear();
			polygon_wm( m_polygonScreen, _screenPoly, _screenTransform );

            mt::box2f bb_screen;
            if( polygon_to_box2f( bb_screen, m_polygonScreen ) == false )
            {
                return false;
            }

            if( mt::is_intersect( bb, bb_screen ) == false )
            {
                return false;
            }

			intersect = boost::geometry::intersects( m_polygonWM, m_polygonScreen );
		}

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testArrow( const mt::mat4f& _transform, Arrow * _arrow, const mt::mat4f& _screenTransform )
	{
		float radius = _arrow->getRadius();

		if( radius < 0.0001f )
		{
			const Polygon & screenPoly = _arrow->getPolygon();

			bool result = this->testPolygon( _transform, screenPoly, _screenTransform );

			return result;
		}
		
		bool result = this->testRadius( _transform, radius, _screenTransform );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform )
	{
        (void)_radius;

        Polygon polygon_point;
        boost::geometry::append( polygon_point, mt::vec2f(0.f, 0.f) ); 

        bool intersect = this->testPolygon( _transform, polygon_point, _screenTransform );

		return intersect;
	}
//#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) <= 0 )
		{
			return;
		}

		VectorVertices::invalidateVertices();

		VectorVertices::TVectorVertex2D & vertices = this->getVertices();

		if( vertices.empty() )
		{
			return;
		}

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject2D( _camera, m_debugMaterial, NULL, 0, &(vertices[0]), vertices.size(), LPT_LINE );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate )
	{
        (void)_invalidate;

		size_t numpoints = boost::geometry::num_points(m_polygon);

		if( numpoints == 0 )
		{
			return;
		}

		_vertices.resize( numpoints + 1 );

		const mt::mat4f & worldMat = this->getWorldMatrix();

		const Polygon::ring_type & ring = m_polygon.outer();

		for( size_t i = 0; i < numpoints; ++i )
		{
			mt::vec2f trP;
			mt::mul_v2_m4( trP, ring[i], worldMat );

			_vertices[i].pos[0] = trP.x;
			_vertices[i].pos[1] = trP.y;
			_vertices[i].pos[2] = 0.f;
			//_vertices[i].pos[3] = 1.f;

			_vertices[i].color = m_debugColor;

            _vertices[i].uv[0] = 0.f;
            _vertices[i].uv[1] = 0.f;

            _vertices[i].uv2[0] = 0.f;
            _vertices[i].uv2[1] = 0.f;
		}

		if( _vertices.size() > 1 )
		{
			std::copy( _vertices.begin(), _vertices.begin() + 1, _vertices.end() - 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		VectorVertices::invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
//#	endif
}
