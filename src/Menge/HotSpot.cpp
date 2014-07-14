#	include "HotSpot.h"

#	include "Interface/MousePickerSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Kernel/Scene.h"
#	include "Layer2D.h"

#	include "Arrow.h"

#	include "Interface/ResourceInterface.h"
#	include "ResourceImage.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
		: m_debugColor(0x00000000)
		, m_invalidatePolygonWM(true)
		, m_outward(false)
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
		this->invalidatePolygonWM();
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & HotSpot::getPolygon() const
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setOutward( bool _value )
	{
		m_outward = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::getOutward() const
	{
		return m_outward;
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
	bool HotSpot::isMousePickerOver() const
	{
		if( m_mousePickerAdapter.isActivePicker() == false )
		{
			return false;
		}

		PickerTrapState * trap = m_mousePickerAdapter.getPicker();

		bool picked = trap->picked;

		return picked;
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrapInterface * HotSpot::getPickerTrap()
	{
		return &m_mousePickerAdapter;
	}
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::onPickerEnter()
    {
		m_debugColor = 0xFFFF0000;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::onPickerLeave()
    {
		m_debugColor = 0xFFFFFFFF;
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
	void HotSpot::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidatePolygonWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::invalidatePolygonWM()
	{
		m_invalidatePolygonWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::updatePolygonWM_()
	{
		m_invalidatePolygonWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		polygon_wm( m_polygonWM, m_polygon, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::activatePicker_()
	{
		if( m_mousePickerAdapter.isActivePicker() == true )
		{
			return;
		}

		m_debugColor = 0xFFFFFFFF;

		m_mousePickerAdapter.activatePicker();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACTIVATE)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::deactivatePicker_()
	{
		if( m_mousePickerAdapter.isActivePicker() == false )
		{
			return;
		}

		m_mousePickerAdapter.deactivatePicker();

		m_debugColor = 0x00000000;

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_localHide( bool _value )
	{
		if( _value == true )
		{
			this->deactivatePicker_();
		}
		else
		{
			if( this->isAfterActive() == true || this->isFreeze() == false )
			{
				this->activatePicker_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_freeze( bool _value )
	{
		Node::_freeze( _value );

		if( _value == true )
		{
			this->deactivatePicker_();
		}
		else
		{
			if( this->isAfterActive() == true || this->isLocalHide() == false )
			{
				this->activatePicker_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_afterActivate()
	{
		if( this->isLocalHide() == false || this->isFreeze() == false )
		{
			this->activatePicker_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		this->deactivatePicker_();
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateBoundingBox( mt::box2f & _boundingBox ) const
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
    bool HotSpot::testPoint( const mt::vec2f & _point )
    {
		const Polygon & polygonWM = this->getPolygonWM();

		mt::box2f bb;
		if( polygon_to_box2f( bb, polygonWM ) == false )
		{
			return false;
		}

		if( mt::is_intersect( bb, _point ) == false )
		{
			return false;
		}

		GeometryPoint p(_point.x, _point.y);

		bool intersect = boost::geometry::intersects( polygonWM, p );

		return intersect != m_outward;
    }
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testRadius( const mt::vec2f & _point, float _radius )
	{
		(void)_radius;

		bool intersect = this->testPoint( _point );

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::vec2f & _point, const Polygon & _screenPoly )
	{
		const Polygon & polygonWM = this->getPolygonWM();

		polygon_transpose( m_polygonWMVM, polygonWM, -_point );

		mt::box2f bb;
		if( polygon_to_box2f( bb, m_polygonWMVM ) == false )
		{
			return false;
		}

		mt::box2f bb_screen;
		if( polygon_to_box2f( bb_screen, _screenPoly ) == false )
		{
			return false;
		}

		if( mt::is_intersect( bb, bb_screen ) == false )
		{
			return false;
		}

		bool intersect = boost::geometry::intersects( m_polygonWMVM, _screenPoly );

		return intersect != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		size_t numpoints = boost::geometry::num_points(m_polygon);

		if( numpoints == 0 )
		{
			return;
		}

		size_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		const mt::mat4f & worldMat = this->getWorldMatrix();

		const Polygon::ring_type & ring = m_polygon.outer();

		for( size_t i = 0; i != numpoints; ++i )
		{
			size_t j = (i + 1) % numpoints;

			mt::vec2f trP0;
			mt::mul_v2_m4( trP0, ring[i], worldMat );

			RenderVertex2D & v0 = vertices[i*2+0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = m_debugColor;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			mt::vec2f trP1;
			mt::mul_v2_m4( trP1, ring[j], worldMat );

			RenderVertex2D & v1 = vertices[i*2+1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = m_debugColor;
			v1.uv.x = 0.f;
			v1.uv.y = 0.f;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
            , vertices
			, vertexCount
			, nullptr
            );
	}
//#	endif
}
