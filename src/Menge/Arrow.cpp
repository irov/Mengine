#	include "Arrow.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "XmlEngine.h"
#	include "Application.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Arrow )
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
	: m_offsetClick(0,0)
	, m_currentHotSpot(0)
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
		SceneNode2D::_update( _timing );

		/*InputEngine * inputEng = Holder<InputEngine>::hostage();

		float mx = inputEng->getMouseX();
		float my = inputEng->getMouseY();
		
		mt::vec2f pos( mx, my );
		setLocalPosition( pos + m_offsetClick );*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		SceneNode2D::_activate();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		setLocalPosition( m_offsetClick );
		m_currentHotSpot = new HotSpot();
		m_currentHotSpot->addPoint( -m_offsetClick );

		m_currentHotSpot->setName("MainHotSpotArrow");

		bool result = this->addChildren( m_currentHotSpot );

		if( result == false )
		{
			return false;
		}

		result = m_currentHotSpot->activate();

		m_window = Holder<Application>::hostage()->getCurrentResolution();
		return true;
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
		SceneNode2D::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ClickOffset", "Value", m_offsetClick );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_renderBegin()
	{
		Holder<RenderEngine>::hostage()
			->beginLayer();

		Viewport viewport;
		viewport.begin = mt::vec2f( 0.f, 0.f );
		viewport.end = mt::vec2f( 1024.f, 768.f );

		Holder<RenderEngine>::hostage()
			->setRenderViewport( viewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_renderEnd()
	{
		Holder<RenderEngine>::hostage()
			->endLayer();
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
	void Arrow::_debugRender()
	{
		
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
		else if( nPos.x > m_window.x )
		{
			nPos.x = m_window.x;
		}
		if( nPos.y < 0.0f )
		{
			nPos.y = 0.0f;
		}
		else if( nPos.y > m_window.y )
		{
			nPos.y = m_window.y;
		}
		setLocalPosition( nPos );
	}
	//////////////////////////////////////////////////////////////////////////
}