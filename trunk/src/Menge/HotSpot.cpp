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
	void HotSpot::activatePicker_()
	{
		if( m_mousePickerAdapter.isActivePicker() == true )
		{
			return;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::deactivatePicker_()
	{
		if( m_mousePickerAdapter.isActivePicker() == false )
		{
			return;
		}

		m_mousePickerAdapter.deactivatePicker();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)( "()" );

		m_debugColor = 0x00000000;
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
			if( this->isActivate() == true || this->isFreeze() == false )
			{
				this->activatePicker_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_freeze( bool _value )
	{
		if( _value == true )
		{
			this->deactivatePicker_();
		}
		else
		{
			if( this->isActivate() == true || this->isLocalHide() == false )
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
    bool HotSpot::testPoint( const mt::mat4f& _transform, const mt::vec2f & _point )
    {
		static Polygon polygonPick;
		polygonPick.clear();

		mt::vec2f v(0.f, 0.f); 
		boost::geometry::append( polygonPick, v );

		bool intersect = this->testPolygon( _transform, _point, polygonPick );

        return intersect;
    }
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testRadius( const mt::mat4f& _transform, const mt::vec2f & _point, float _radius )
	{
		(void)_radius;
		//static Polygon polygonPick;
		//polygonPick.clear();

		//float half_radius = _radius * 0.5f;
		//boost::geometry::append( polygonPick, mt::vec2f(-half_radius, -half_radius) );
		//boost::geometry::append( polygonPick, mt::vec2f(half_radius, -half_radius) );
		//boost::geometry::append( polygonPick, mt::vec2f(half_radius, half_radius) );
		//boost::geometry::append( polygonPick, mt::vec2f(-half_radius, half_radius) );
		//
		//boost::geometry::correct( polygonPick );

		//bool intersect = this->testPolygon( _transform, _point, polygonPick );
		bool intersect = this->testPoint( _transform, _point );

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::mat4f& _transform, const mt::vec2f & _point, const Polygon & _screenPoly )
	{
		static Polygon polygonWM;
		polygonWM.clear();

		const mt::mat4f & wm = this->getWorldMatrix();

		polygon_wm( polygonWM, m_polygon, wm );				

		static Polygon polygonWMVM;
		polygonWMVM.clear();

		polygon_wm( polygonWMVM, polygonWM, _transform );

		mt::box2f bb;
		if( polygon_to_box2f( bb, polygonWMVM ) == false )
		{
			return false;
		}

		static Polygon polygonScreen;
		polygonScreen.clear();

		polygon_transpose( polygonScreen, _screenPoly, _point );

		mt::box2f bb_screen;
		if( polygon_to_box2f( bb_screen, polygonScreen ) == false )
		{
			return false;
		}

		if( mt::is_intersect( bb, bb_screen ) == false )
		{
			return false;
		}

		bool intersect = boost::geometry::intersects( polygonWM, polygonScreen );

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) == 0 )
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

		const RenderMaterial * debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial, NULL, 0
            , vertices
			, vertexCount
            );
	}
//#	endif
}
