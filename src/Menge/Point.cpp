#	include "Point.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "HotSpot.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Point )
	//////////////////////////////////////////////////////////////////////////
	Point::Point()
	{
		this->setHandler( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_update( float _timing )
	{
		SceneNode2D::_update( _timing );

		const Viewport & viewport =
			Holder<RenderEngine>::hostage()
			->getRenderViewport();

		Holder<MousePickerSystem>::hostage()
			->regTrap( this, viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( HotSpot * _hotspot )
	{
		const mt::vec2f& pos = getWorldPosition();
	//	printf("%f \n", m_offset.x);
		bool result = _hotspot->testPoint( pos,m_offset );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::onLeave()
	{
		callEvent( "LEAVE", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::onEnter()
	{
		callEvent( "ENTER", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::pick( const mt::vec2f& _offset, HotSpot * _hotspot )
	{
		m_offset = _offset;
		//printf("%f \n", _offset.x);
		return true;
	}
	bool Point::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	bool Point::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	bool Point::handleMouseMove( int _x, int _y, int _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
}