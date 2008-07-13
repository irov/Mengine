#	include "Arrow.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Arrow )
		//////////////////////////////////////////////////////////////////////////
		Arrow::Arrow()
		: m_offsetClick(0,0)
		, m_currentHotSpot(0)
		, m_hided(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_offsetClick = _offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getOffsetClick() const
	{
		return m_offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_update( float _timing )
	{
		Node::_update( _timing );

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		float mx = inputEng->getMouseX();
		float my = inputEng->getMouseY();

		mt::vec2f pos( mx, my );
		setLocalPosition( pos + m_offsetClick );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		Node::_activate();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		m_currentHotSpot = new HotSpot();
		m_currentHotSpot->addPoint( -m_offsetClick );
		m_currentHotSpot->setName("MainHotSpotArrow");

		bool result = this->addChildren( m_currentHotSpot );

		if( result == false )
		{
			return false;
		}

		result = m_currentHotSpot->activate();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Arrow" )
			{
				XML_PARSE_ELEMENT( this, &Arrow::loaderArrow_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////	
	void Arrow::loaderArrow_( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ClickOffset", "Value", m_offsetClick );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::addHotSpot( HotSpot * _hotspot )
	{
		if( m_currentHotSpot != NULL )
		{
			this->removeChildren( m_currentHotSpot );
		}

		bool result = this->addChildren( _hotspot );

		if( result == false )
		{
			assert(!"can't add hotspot to arrow");
		}

		m_currentHotSpot = _hotspot;
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot * Arrow::getCurrentHotSpot() const
	{
		return m_currentHotSpot;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMouseMove( int _dx, int _dy )
	{
		const mt::vec2f& pos = getLocalPosition();
		mt::vec2f nPos = mt::vec2f( pos.x + _dx, pos.y + _dy );
		if( nPos.x < 0.0f )
		{
			nPos.x = 0.0f;
		}
		else if( nPos.x > 1024.0f )
		{
			nPos.x = 1024.0f;
		}
		if( nPos.y < 0.0f )
		{
			nPos.y = 0.0f;
		}
		else if( nPos.y > 768.0f )
		{
			nPos.y = 768.0f;
		}
		setLocalPosition( nPos );
	}

	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMouseLeave()
	{
		Node/*Renderable*/::hide( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMouseEnter()
	{
		if( !m_hided )
		{
			Node/*Renderable*/::hide( false );
		}
	}
}