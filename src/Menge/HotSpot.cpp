#	include "HotSpot.h"

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
	HotSpot::HotSpot()
		: m_debugColor(0x00000000)		
		, m_outward(false)
		, m_invalidatePolygonWM(true)
		, m_picker(nullptr)
		, m_defaultHandle(true)		
		, m_onMouseMoveEvent(false)
		, m_onMouseEnterEvent(false)
		, m_onMouseLeaveEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{
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
	bool HotSpot::isMousePickerOver() const
	{
		if( m_picker == nullptr )
		{
			return false;
		}
		
		bool picked = m_picker->picked;

		return picked;
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrapInterface * HotSpot::getPickerTrap()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setDefaultHandle( bool _handle )
	{
		m_defaultHandle = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::getDefaultHandle() const
	{
		return m_defaultHandle;
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

		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_MOUSE_WHEEL, ("onHandleMouseWheel"), _listener );

		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener, &m_onMouseMoveEvent );
		this->registerEvent( EVENT_MOUSE_LEAVE, ("onHandleMouseLeave"), _listener, &m_onMouseLeaveEvent );
		this->registerEvent( EVENT_MOUSE_ENTER, ("onHandleMouseEnter"), _listener, &m_onMouseEnterEvent );

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
	void HotSpot::_invalidHierarchy()
	{
		//HotSpot * parent = this->findTypeParent<HotSpot>();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::invalidatePolygonWM()
	{
		m_invalidatePolygonWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::updatePolygonWM_() const
	{
		m_invalidatePolygonWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		polygon_wm( m_polygonWM, m_polygon, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	PickerTrapState * HotSpot::getPicker() const
	{
		return m_picker;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::activatePicker_()
	{
		if( m_picker != nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' alredy activate picker"
				, this->getName().c_str()
				);

			return;
		}

		MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
			->getMousePickerSystem();

		if( mousePickerSystem == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' invalid get mouse picker system"
				, this->getName().c_str()
				);

			return;
		}

		m_debugColor = 0xFFFFFFFF;

		m_picker = mousePickerSystem->regTrap( this );

		mousePickerSystem->updateTrap();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACTIVATE)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::deactivatePicker_()
	{
		if( m_picker == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' alredy deactivate picker"
				, this->getName().c_str()
				);

			return;
		}

		MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
			->getMousePickerSystem();

		if( mousePickerSystem == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' invalid get mouse picker system"
				, this->getName().c_str()
				);

			return;
		}

		mousePickerSystem->unregTrap( m_picker );
		m_picker = nullptr;

		mousePickerSystem->updateTrap();

		m_debugColor = 0x00000000;

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onMouseLeave()
	{
		m_debugColor = 0xFFFFFFFF;

		if( m_onMouseLeaveEvent == true )
		{
			EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOUSE_LEAVE)( "(O)"
				, this->getEmbed() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::onMouseEnter( const mt::vec2f & _point )
	{
		m_debugColor = 0xFFFF0000;

		bool handle = m_defaultHandle;

		if( m_onMouseEnterEvent == true )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_ENTER)( handle, m_defaultHandle, "(Off)"
				, this->getEmbed() 
				, _point.x
				, _point.y
				);
		}		

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		bool handle = false;
				
		EVENTABLE_ASK(m_serviceProvider, this, EVENT_KEY)( handle, m_defaultHandle, "(OIffIOO)"
			, this->getEmbed()
			, _key
			, _point.x
			, _point.y
			, _char
			, pybind::get_bool(_isDown)
			, pybind::get_bool(_repeating)
			);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON)( handle, m_defaultHandle, "(OIffIO)"
			, this->getEmbed()
			, _touchId
			, _point.x
			, _point.y
			, _button
			, pybind::get_bool(_isDown) 
			);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool HotSpot::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN)( "(OIffIO)"
			, this->getEmbed()
			, _touchId
			, _point.x
			, _point.y
			, _button
			, pybind::get_bool(_isDown) 
			);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{		
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOUSE_BUTTON_END)( "(OIffIO)"
			, this->getEmbed()
			, _touchId
			, _point.x
			, _point.y
			, _button
			, pybind::get_bool(_isDown) 
			);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		bool handle = m_defaultHandle;

		if( m_onMouseMoveEvent == true )
		{			
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_MOVE)( handle, m_defaultHandle, "(OIffff)"
				, this->getEmbed()
				, _touchId
				, _point.x
				, _point.y
				, _x
				, _y
				);
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		bool handle = false;
				
		EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_WHEEL)( handle, m_defaultHandle, "(OIffi)"
			, this->getEmbed()
			, _touchId
			, _point.x
			, _point.y
			, _wheel 
			);

		return handle;
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
    bool HotSpot::testPoint( const mt::vec2f & _point ) const
    {
		const Polygon & polygonWM = this->getPolygonWM();

		mt::box2f bb;
		if( polygon_to_box2f( bb, polygonWM ) == false )
		{
			return m_outward;
		}

		if( mt::is_intersect( bb, _point ) == false )
		{
			return m_outward;
		}

		GeometryPoint p(_point.x, _point.y);

		bool intersect = boost::geometry::intersects( polygonWM, p );

		return intersect != m_outward;
    }
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testRadius( const mt::vec2f & _point, float _radius ) const
	{
		(void)_radius;

		bool intersect = this->testPoint( _point );

		return intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::vec2f & _point, const Polygon & _screenPoly ) const
	{
		const Polygon & polygonWM = this->getPolygonWM();

		polygon_transpose( m_polygonWMVM, polygonWM, -_point );

		mt::box2f bb;
		if( polygon_to_box2f( bb, m_polygonWMVM ) == false )
		{
			return m_outward;
		}

		mt::box2f bb_screen;
		if( polygon_to_box2f( bb_screen, _screenPoly ) == false )
		{
			return m_outward;
		}

		if( mt::is_intersect( bb, bb_screen ) == false )
		{
			return m_outward;
		}

		bool intersect = boost::geometry::intersects( m_polygonWMVM, _screenPoly );

		return intersect != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Arrow * _arrow )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		Layer * layer = this->getLayer();

		if( layer == nullptr )
		{
			return false;
		}

		HotspotMousePickerVisitor mp(this, _viewport, _camera, _point, _arrow);
		bool result = mp.test( layer );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * HotSpot::getPickerEmbed()
	{
		PyObject * embed = this->getEmbed();

		return embed;
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
