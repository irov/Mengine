#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Sprite.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListener( false )
	, m_globalKeyEventListener( false )
	, m_onLeaveEvent( false )
	, m_onEnterEvent( false )
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
	bool HotSpot::_pickerActive()
	{
		return this->isActivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onLeave()
	{
		callEvent( "LEAVE", "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onEnter()
	{
		callEvent( "ENTER", "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_polygon.add_point( mt::vec2f( _p.x * m_scale.x, _p.y * m_scale.y ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear_points();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( HotSpot * _hotspot )
	{
		if( _hotspot == this )
		{
			return false;
		}

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		const mt::vec2f & dirA = this->getWorldDirection();
		const mt::vec2f & posA = this->getScreenPosition();

		const mt::vec2f & dirB = _hotspot->getWorldDirection();
		const mt::vec2f & posB = _hotspot->getScreenPosition();

		bool is_intersect = mt::intersect_poly_poly( 
			m_polygon, _hotspot->m_polygon, 
			dirA, posA, dirB, posB );

		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( XmlElement * _xml)
	{
		Node::loader(_xml);

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
	bool HotSpot::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(OIIb)", this->getEmbedding(), _key, _char, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_BUTTON", "(OIb)", this->getEmbedding(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_MOVE", "(Offi)", this->getEmbedding(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_BUTTON", "(OIb)", this->getEmbedding(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_MOUSE_MOVE", "(Offi)", this->getEmbedding(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "GLOBAL_KEY", "(OIIb)", this->getEmbedding(), _key, _char, _isDown );
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
	void HotSpot::_setListener()
	{
		Node::_setListener();

		registerEvent( "KEY", "onHandleKeyEvent", m_listener );
		registerEvent( "MOUSE_BUTTON", "onHandleMouseButtonEvent", m_listener );
		registerEvent( "MOUSE_MOVE", "onHandleMouseMove", m_listener );

		registerEvent( "GLOBAL_MOUSE_BUTTON", "onGlobalHandleMouseButtonEvent", m_listener );
		registerEvent( "GLOBAL_MOUSE_MOVE", "onGlobalHandleMouseMove", m_listener );
		registerEvent( "GLOBAL_KEY", "onGlobalHandleKeyEvent", m_listener );
		
		m_onLeaveEvent = registerEvent( "LEAVE", "onLeave", m_listener );
		m_onEnterEvent = registerEvent( "ENTER", "onEnter", m_listener );	
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
		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		const mt::vec2f & direction = this->getWorldDirection();
		const mt::vec2f & position = this->getScreenPosition(  );

		bool result = mt::is_point_inside_polygon(m_polygon, _p, position, direction);

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setScale( const mt::vec2f& _scale )
	{
		for( mt::polygon::TVectorPoints::size_type 
			it = 0,
			it_end = m_polygon.num_points();
		it != it_end; 
		++it )
		{
			m_polygon[it].x = m_polygon[it].x / m_scale.x * _scale.x;
			m_polygon[it].y = m_polygon[it].y / m_scale.y * _scale.y;
		}

		m_scale = _scale;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_render( const Viewport & _viewport, bool _enableDebug )
	{
		if( _enableDebug )
		{
			mt::polygon::TVectorPoints::size_type pointCount = m_polygon.num_points();

			for(int i = 0; i < pointCount; i++)
			{
				mt::vec2f beg = m_polygon[i];
				mt::vec2f end = m_polygon[(i+1) % pointCount];

				mt::vec2f pt1, pt2;
				const mt::mat3f& wm = getWorldMatrix();
				mt::mul_v2_m3( pt1, beg, wm );
				mt::mul_v2_m3( pt2, end, wm );

				Holder<RenderEngine>::hostage()->renderLine(0xFFFF0000,pt1,pt2);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}