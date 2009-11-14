#	include "Arrow.h"

#	include "Application.h"
#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "Game.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Arrow)
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

		InputEngine * inputEngine = Holder<InputEngine>::hostage();
		const mt::vec2f & mx = inputEngine->getMousePosition();

		float vpdx = 1.0f;
		float vpdy = 1.0f;

		float dx = 0.0f;
		float dy = 0.0f;

		Game * game = Holder<Game>::hostage();
		RenderEngine * renderEngine = Holder<RenderEngine>::hostage();

		if( renderEngine != NULL )
		{
			const Resolution& contentResolution = game->getContentResolution();
			const Viewport & viewport = renderEngine->getRenderViewport();
			vpdx = static_cast<float>( contentResolution[0] ) / ( viewport.end.x - viewport.begin.x );
			vpdy = static_cast<float>( contentResolution[1] ) / ( viewport.end.y - viewport.begin.y );
			dx = -viewport.begin.x;
			dy = -viewport.begin.y;
		}
		float fx =  vpdx * (mx.x + dx);
		float fy =  vpdy * (mx.y + dy);

		setLocalPosition( mt::vec2f(fx, fy) );
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
		m_currentHotSpot->setName( "MainHotSpotArrow" );

		bool result = this->addChildren( m_currentHotSpot );

		if( result == false )
		{
			return false;
		}

		result = m_currentHotSpot->activate();

		bool cursorMode = Application::hostage()
							->getCursorMode();
		setCursorMode( cursorMode );
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
			m_currentHotSpot->destroy();
		}

		bool result = this->addChildren( _hotspot );
		_hotspot->activate();

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
	void Arrow::onMouseMove( float _dx, float _dy )
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMouseLeave()
	{
		Node::hide( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMouseEnter()
	{
		if( !m_hided )
		{
			Node::hide( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::hide( bool _value )
	{
		Node::hide( _value );
		m_hided = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Arrow::getScreenPosition()
	{
		return getLocalPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setWindow( const Resolution & _window )
	{
		m_resolution = _window;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_release()
	{
		if( m_currentHotSpot != NULL )
		{
			this->removeChildren( m_currentHotSpot );
			m_currentHotSpot->destroy();
			m_currentHotSpot = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setCursorMode( bool _mode )
	{
		if( m_children.empty() == false )
		{
			Node* mainCursor = this->getChildren( "Default", false );
			if( mainCursor != NULL )
			{
				_mode ? mainCursor->disable() : mainCursor->enable();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
