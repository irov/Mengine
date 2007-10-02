#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "MousePickerSystem.h"

#	include "InputEngine.h"
#	include "ScriptEngine.h"
#	include "RenderEngine.h"

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_handler( 0 )
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
		callEvent( "LEAVE", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onEnter()
	{
		callEvent( "ENTER", "()" );
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
		//RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		//size_t size = m_poligon.num_points();

		//for( size_t i = 0; i < size; ++i )
		//{
		//	const mt::mat3f & wm = getWorldMatrix();
		//	
		//	mt::vec2f b;
		//	mt::vec2f e;

		//	mt::mul_v2_m3( b, m_poligon[ i ], wm );
		//	mt::mul_v2_m3( e, m_poligon[ (i+1) % size ], wm );

		//	renderEng->drawLine2D( b, e, 0xffff00ff );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( size_t _key, bool _isDown )
	{
		bool result = false;
		askEvent( result, "KEY", "(Ib)", _key, _isDown );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool result = false;
		askEvent( result, "MOUSE_BUTTON", "(Ib)", _button, _isDown );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( int _x, int _y, int _whell )
	{
		bool result = false;
		askEvent( result, "MOUSE_MOVE", "(fff)", _x, _y, _whell );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setInputHandler( PyObject * _handler )
	{
		ScriptEngine::incref( _handler );

		m_handler = _handler;

		this->registerEvent("KEY", m_handler, "onHandleKeyEvent" );
		this->registerEvent("MOUSE_BUTTON", m_handler, "onHandleMouseButtonEvent" );
		this->registerEvent("MOUSE_MOVE", m_handler, "onHandleMouseMove" );

		this->registerEvent("LEAVE", m_handler, "onLeave" );
		this->registerEvent("ENTER", m_handler, "onEnter" );		
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
	void HotSpot::_update( float _timing )
	{
		Holder<MousePickerSystem>::hostage()
			->regTrap( this );
	}
}