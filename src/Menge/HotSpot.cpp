#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "InputEngine.h"

#	include "ScriptEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_handle( true )
	, m_mouseLeftClick( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{

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

			renderEng->drawLine( b, e, 0xffff00ff );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setMouseLeftClickEvent( PyObject * _object )
	{
		registerEvent( "MOUSE_LEFT_CLICK", _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::update( float _timing )
	{
		const mt::vec3f & v = Holder<InputEngine>::hostage()
			->getPosition();
		
		bool mlReleased = false;
		
		//Holder<InputEngine>::hostage()
		//	->isButton( MOUSE_LEFT, DI_RELEASED );

		if( mlReleased == true )
		{	
			if( testPoint( v.v2 ) == false )
			{
				return;
			}

			if( PyObject * eventMouseLeftClick = getEvent( "MOUSE_LEFT_CLICK" ) )
			{
				Holder<ScriptEngine>::hostage()
					->callFunction( eventMouseLeftClick );
			}
		}
	}
}