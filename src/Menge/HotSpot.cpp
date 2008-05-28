#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Sprite.h"

//#define DEBUG_RENDER

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListener( false )
	, m_globalKeyEventListener( false )
	, m_scale( 1.0f, 1.0f )
	{
		this->setHandler( this );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{
		Holder<Player>::hostage()
			->unregGlobalMouseEventable( this );

		Holder<Player>::hostage()
			->unregGlobalKeyEventable( this );

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
		//!!!!!!!
		m_points.push_back( mt::vec2f( _p.x * m_scale.x, _p.y * m_scale.y ) );
		//m_polygon.add_point( mt::vec2f( _p.x * m_scale.x, _p.y * m_scale.y ) );

#ifdef DEBUG_RENDER
		Sprite* point = new Sprite();
		point->setImageResource( "AttrBad" );
		//point->setName( "point" + std::string(::itoa ) );
		point->activate();
		point->enable();
		point->compile();

		if( m_polygon.num_points() > 1 )
		{
			point->setLocalPosition( m_polygon[ m_polygon.num_points() - 2 ] );
			float sx = ( m_polygon.back() - m_polygon[ m_polygon.num_points() - 2 ]).length();
			point->setScale( mt::vec2f( sx / 25.0f, 1.0f) );
			point->setLocalDirection( mt::norm_v2( m_polygon.back() - m_polygon[ m_polygon.num_points() - 2 ] ) );

			sx = ( m_polygon.back() - m_polygon[0]).length();
			(*m_listChildren.begin())->setScale( mt::vec2f( sx / 25.0f, 1.0f ));
			(*m_listChildren.begin())->setLocalDirection( mt::norm_v2( m_polygon.back() - m_polygon[ 0 ] ) );
		}
		else
			point->setLocalPosition( m_polygon.back() );

		activate();
		
		addChildren( point );
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		//m_polygon.clear_points();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( HotSpot * _hotspot )
	{
		if( _hotspot == this )
		{
			return false;
		}
		
		const mt::vec2f & dirA = this->getLocalDirection();
		const mt::vec2f & posA = this->getScreenPosition();

		const mt::vec2f & dirB = _hotspot->getLocalDirection();
		const mt::vec2f & posB = _hotspot->getScreenPosition();

		return mt::intersect_poly_poly(m_polygons,_hotspot->m_polygons,posA,dirA,posB,dirB);
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( XmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Point")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &HotSpot::addPoint );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_compile()
	{
		if(mt::is_convex_pointsoup(m_points) == false)
		{
			mt::decompose_concave(m_points,m_polygons);
		}
		else
		{
			m_polygons.push_back(mt::convexpoly2(m_points));
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(OIIb)", this->getScript(), _key, _char, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_BUTTON", "(OIb)", this->getScript(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_MOVE", "(Offi)", this->getScript(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_BUTTON", "(OIb)", this->getScript(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_MOVE", "(Offi)", this->getScript(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_KEY", "(OIIb)", this->getScript(), _key, _char, _isDown );
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
	void HotSpot::_onSetListener()
	{
		SceneNode2D::_onSetListener();

		registerEventListener( "KEY", "onHandleKeyEvent", m_listener );
		registerEventListener( "MOUSE_BUTTON", "onHandleMouseButtonEvent", m_listener );
		registerEventListener( "MOUSE_MOVE", "onHandleMouseMove", m_listener );

		registerEventListener( "GLOBAL_MOUSE_BUTTON", "onGlobalHandleMouseButtonEvent", m_listener );
		registerEventListener( "GLOBAL_MOUSE_MOVE", "onGlobalHandleMouseMove", m_listener );
		registerEventListener( "GLOBAL_KEY", "onGlobalHandleKeyEvent", m_listener );
		
		registerEventListener( "LEAVE", "onLeave", m_listener );
		registerEventListener( "ENTER", "onEnter", m_listener );		
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
		const mt::vec2f & direction = this->getLocalDirection();
		const mt::vec2f & position = this->getScreenPosition();

		return mt::is_point_inside_polygon(m_polygons, _p, position, direction);
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender()
	{
		for(int j = 0; j < m_polygons.size(); j++)
		{
			for(int i = 0; i < m_polygons[j].num_points(); i++)
			{
				mt::vec2f beg = m_polygons[j][i];
				mt::vec2f end = m_polygons[j][(i+1) % m_polygons[j].num_points()];

				beg+=getWorldPosition();
				end+=getWorldPosition();

				Holder<RenderEngine>::hostage()->renderLine(0xFFFF0000,beg,end);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setScale( const mt::vec2f& _scale )
	{
		for(int i = 0; i < m_polygons.size(); i++)
		{
			for( mt::convexpoly2::TVectorPoints::size_type 
				it = 0,
				it_end = m_polygons[i].num_points();
			it != it_end; 
			++it )
			{
				m_polygons[i][it].x = m_polygons[i][it].x / m_scale.x * _scale.x;
				m_polygons[i][it].y = m_polygons[i][it].y / m_scale.y * _scale.y;
			}
		}

#ifdef DEBUG_RENDER
		TListChildren::iterator it = m_listChildren.begin();
		for(; it != m_listChildren.end(); it++)
		{
			mt::vec2f pos = (*it)->getLocalPosition();
			pos.x = pos.x / m_scale.x * _scale.x;
			pos.y = pos.y / m_scale.y * _scale.y;
			(*it)->setLocalPosition( pos );
			mt::vec2f scl = (*it)->getScale();
			scl.x *= _scale.x;
			scl.y *= _scale.y;
			(*it)->setScale( scl );
		}
#endif
		m_scale = _scale;
	}
}