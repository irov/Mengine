#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "MousePickerSystem.h"

#	include "InputEngine.h"
#	include "ScriptEngine.h"
#	include "RenderEngine.h"

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"

#	include "Event.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_handler( 0 )
	, m_handle( true )
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
		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_handler, "onLeave" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_handler, "onLeave", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onEnter()
	{
		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_handler, "onEnter" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_handler, "onEnter", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setHandle( bool _handle )
	{
		m_handle = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_poligon.add_point(_p);
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		size_t size = m_poligon.num_points();

		if(size == 0)
		{
			return false;
		}

		const mt::mat3f & wm = getWorldMatrix();

		size_t intersect_counter = 0;

		mt::vec2f prev;
		mt::mul_v2_m3( prev, m_poligon[ size - 1], wm );

		for ( size_t i = 0; i < size; ++i )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, m_poligon[ i ], wm );

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
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		size_t size = m_poligon.num_points();

		for( size_t i = 0; i < size; ++i )
		{
			const mt::mat3f & wm = getWorldMatrix();
			
			mt::vec2f b;
			mt::vec2f e;

			mt::mul_v2_m3( b, m_poligon[ i ], wm );
			mt::mul_v2_m3( e, m_poligon[ (i+1) % size ], wm );

			renderEng->drawLine2D( b, e, 0xffff00ff );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( size_t _key, bool _isDown )
	{
		return Holder<ScriptEngine>::hostage()
			->handleKeyEvent( m_handler, _key, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return Holder<ScriptEngine>::hostage()
			->handleMouseButtonEvent( m_handler, _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( float _x, float _y, float _whell )
	{
		return Holder<ScriptEngine>::hostage()
			->handleMouseMove( m_handler, _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setInputHandler( PyObject * _handler )
	{
		ScriptEngine::incref( _handler );

		m_handler = _handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		return SceneNode2D::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::update( float _timing )
	{
		Holder<MousePickerSystem>::hostage()
			->regTrap( this );

		const mt::vec3f & v = Holder<InputEngine>::hostage()
			->getPosition();
		
		bool mlReleased = 
			Holder<InputEngine>::hostage()
			->isButton( 0, 1 );

		if( mlReleased == true )
		{	
			if( testPoint( v.v2 ) == false )
			{
				return;
			}

			if( Event * eventLC = getEvent( "MOUSE_LEFT_CLICK" ) )
			{
				eventLC->call( "()" );
			}
		}
	}
}