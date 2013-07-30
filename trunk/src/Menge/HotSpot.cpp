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

		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear();

		this->invalidateBoundingBox();
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

		if( m_enable == true )
		{
			m_debugColor = 0xA0FFFFFF;
		}
		else
		{
			m_debugColor = 0x20FFFFFF;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		m_mousePickerAdapter.deactivatePicker();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)( "()" );

		m_debugColor = 0x00000000;
		
		Node::_deactivate();
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
		m_polygonWM.clear();
		polygon_wm( m_polygonWM, m_polygon, _transform );

        mt::box2f bb;
        if( polygon_to_box2f( bb, m_polygonWM ) == false )
        {
            return false;
        }

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

		bool intersect = boost::geometry::intersects( m_polygonWM, m_polygonScreen );
        
		return intersect;
	}
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::testPoint( const mt::mat4f& _transform, const mt::vec2f & _p, const mt::mat4f& _screenTransform )
    {
        m_polygonWM.clear();
        polygon_wm( m_polygonWM, m_polygon, _transform );

        mt::box2f bb;
        if( polygon_to_box2f( bb, m_polygonWM ) == false )
        {
            return false;
        }

        mt::vec2f wmp;
        mt::mul_v2_m4( wmp, _p, _screenTransform );
        
        if( mt::is_intersect( bb, wmp ) == false )
        {
            return false;
        }

        m_polygonScreen.clear();
        boost::geometry::append(m_polygonScreen, wmp);

        bool intersect = boost::geometry::intersects( m_polygonWM, m_polygonScreen );

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
                
        mt::vec2f point(0.f, 0.f);

        bool intersect = this->testPoint( _transform, point, _screenTransform );

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) <= 0 )
		{
			return;
		}

		this->updateVertices_();

		if( m_vertexDebugPolygon.empty() == true )
		{
			return;
		}

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _camera, m_debugMaterial, NULL, 0
            , &m_vertexDebugPolygon[0], m_vertexDebugPolygon.size()
            );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::updateVertices_()
	{
        m_vertexDebugPolygon.clear();        

		size_t numpoints = boost::geometry::num_points(m_polygon);

		if( numpoints == 0 )
		{
			return;
		}

		m_vertexDebugPolygon.resize( numpoints + 1 );

		const mt::mat4f & worldMat = this->getWorldMatrix();

		const Polygon::ring_type & ring = m_polygon.outer();

		for( size_t i = 0; i != numpoints; ++i )
		{
			mt::vec2f trP;
			mt::mul_v2_m4( trP, ring[i], worldMat );

			m_vertexDebugPolygon[i].pos[0] = trP.x;
			m_vertexDebugPolygon[i].pos[1] = trP.y;
			m_vertexDebugPolygon[i].pos[2] = 0.f;
			//_vertices[i].pos[3] = 1.f;

			m_vertexDebugPolygon[i].color = m_debugColor;

            m_vertexDebugPolygon[i].uv[0] = 0.f;
            m_vertexDebugPolygon[i].uv[1] = 0.f;

            m_vertexDebugPolygon[i].uv2[0] = 0.f;
            m_vertexDebugPolygon[i].uv2[1] = 0.f;
		}

		if( m_vertexDebugPolygon.size() > 2 )
		{
			std::copy( m_vertexDebugPolygon.begin(), m_vertexDebugPolygon.begin() + 1, m_vertexDebugPolygon.end() - 1 );
		}
	}
//#	endif
}
