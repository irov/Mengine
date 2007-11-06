#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "MousePickerSystem.h"

#	include "InputEngine.h"
#	include "ScriptEngine.h"
#	include "RenderEngine.h"

#	include "Player.h"

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListenerEnable(false)
	{
		this->setHandler( this );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( float _x, float _y )
	{
		return this->testPoint( mt::vec2f( _x, _y ) );
	}
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
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		size_t size = m_polygon.num_points();

		if(size == 0)
		{
			return false;
		}

		const mt::mat3f & wm = getWorldMatrix();

		size_t intersect_counter = 0;

		mt::vec2f prev;
		mt::mul_v2_m3( prev, m_polygon[ size - 1], wm );

		for ( size_t i = 0; i < size; ++i )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, m_polygon[ i ], wm );

			if (( point.y > _p.y) ^ (prev.y > _p.y))
			{
				if (prev.x + (_p.y - prev.y) / (point.y - prev.y) * (point.x - prev.x) > _p.x)
				{
					++intersect_counter;
				}
			}
			prev = point;
		}

		return intersect_counter & 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Point")
			{
				mt::vec2f point;
				XML_VALUE_ATTRIBUTE("Value", point);
				addPoint(point);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender()
	{
		//RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		//size_t size = m_polygon.num_points();

		//for( size_t i = 0; i < size; ++i )
		//{
		//	const mt::mat3f & wm = getWorldMatrix();
		//	
		//	mt::vec2f b;
		//	mt::vec2f e;

		//	mt::mul_v2_m3( b, m_polygon[ i ], wm );
		//	mt::mul_v2_m3( e, m_polygon[ (i+1) % size ], wm );

		//	renderEng->drawLine2D( b, e, 0xffff00ff );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( size_t _key, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(OIb)", _key, _isDown );
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
			askEvent( handle, "MOUSE_MOVE", "(Offf)", this->getScript(), _x, _y, _whell );
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
			askEvent( handle, "GLOBAL_MOUSE_MOVE", "(Offf)", this->getScript(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalMouseEvent( bool _value )
	{
		m_globalMouseEventListenerEnable = _value;

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
	void HotSpot::_update( size_t _timing, const Viewport & _viewport )
	{
		Holder<MousePickerSystem>::hostage()
			->regTrap( this, _viewport );
	}
}