#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Camera2D.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListener( false )
	, m_globalKeyEventListener( false )
	{
		this->setHandler( this );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onLeave()
	{
		callEvent( "LEAVE", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onEnter()
	{
		callEvent( "ENTER", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_polygon.add_point(_p);
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear_points();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( const mt::vec2f& _offset, HotSpot * _hotspot )
	{
		bool is_intersect = false;

		const mt::mat3f & wmA = getWorldMatrix();
		const mt::mat3f & wmB = _hotspot->getWorldMatrix();

	//	printf("%f \n", _offset.x);
		is_intersect = mt::intersect_poly_poly( m_polygon, _hotspot->m_polygon, wmA, wmB, _offset );

	//	if(is_intersect)
	//	{
//			printf("true\n");
	//	}
	
		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		mt::vec2f point;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Point")
			{
				XML_VALUE_ATTRIBUTE("Value", point);
				addPoint( point );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( size_t _key, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(OIb)", this->getScript(), _key, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_BUTTON", "(OIb)", this->getScript(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( int _x, int _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_MOVE", "(Oiii)", this->getScript(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_BUTTON", "(OIb)", this->getScript(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseMove( int _x, int _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_MOVE", "(Oiii)", this->getScript(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalMouseEvent( bool _value )
	{
		m_globalMouseEventListener = _value;

		if( _value )
		{
			Holder<Player>::hostage()
				->regGlobalMouseEventable( this );
		}
		else
		{
			Holder<Player>::hostage()
				->unregGlobalMouseEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalKeyEvent( bool _value )
	{
		m_globalKeyEventListener = _value;

		if( _value )
		{
			Holder<Player>::hostage()
				->regGlobalKeyEventable( this );
		}
		else
		{
			Holder<Player>::hostage()
				->unregGlobalKeyEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setHotspotListener( PyObject * _listener )
	{
		registerEventListener( "KEY", "onHandleKeyEvent", _listener );
		registerEventListener( "MOUSE_BUTTON", "onHandleMouseButtonEvent", _listener );
		registerEventListener( "MOUSE_MOVE", "onHandleMouseMove", _listener );

		registerEventListener( "GLOBAL_MOUSE_BUTTON", "onGlobalHandleMouseButtonEvent", _listener );
		registerEventListener( "GLOBAL_MOUSE_MOVE", "onGlobalHandleMouseMove", _listener );

		registerEventListener( "LEAVE", "onLeave", _listener );
		registerEventListener( "ENTER", "onEnter", _listener );		
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_update( float _timing )
	{
		const Viewport & viewport =
			Holder<RenderEngine>::hostage()
			->getRenderViewport();

		Holder<MousePickerSystem>::hostage()
			->regTrap( this, viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		mt::mat3f wm = getWorldMatrix();

		// DIRTY HACK. FIX.
		
		{
			Camera2D * camera = Holder<Player>::hostage()
				->getRenderCamera2D();
			mt::vec2f camera_position = camera->getWorldPosition();
			camera_position-=mt::vec2f(512,384);
			wm.v2.v2 += camera_position; 
		}

		bool result = mt::is_point_inside_polygon( m_polygon, _p, wm );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
}