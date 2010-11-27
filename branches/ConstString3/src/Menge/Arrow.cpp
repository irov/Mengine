#	include "Arrow.h"

#	include "Application.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "Game.h"
#	include "Player.h"

#	include "BinParser.h"

#	include "NodeManager.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
		: m_offsetClick(0.f,0.f)
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
	Scene * Arrow::getScene()
	{
		Scene * scene = Player::get()
			->getCurrentScene();

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_update( float _timing )
	{
		Node::_update( _timing );

		const mt::vec2f & mx = 
			InputEngine::get()->getMousePosition();

		float vpdx = 1.0f;
		float vpdy = 1.0f;

		float dx = 0.0f;
		float dy = 0.0f;

		Game * game = Game::get();
		RenderEngine * renderEngine = RenderEngine::get();

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

		this->setLocalPosition( mt::vec2f(fx, fy) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		bool cursorMode = 
			Application::get()->getCursorMode();

		setCursorMode( cursorMode );

		if( m_polygon.num_points() == 0 )
		{
			m_polygon.add_point( mt::vec2f(0.f,0.f) );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::loader( BinParser * _parser )
	{
		Node::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ClickOffset_Value, m_offsetClick );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Polygon_Point, &Arrow::addPoint_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::addPoint_( const mt::vec2f & _v )
	{
		m_polygon.add_point( _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setPolygon( const mt::polygon & _polygon )
	{
		m_polygon = _polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::polygon & Arrow::getPolygon() const
	{
		return m_polygon;
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
	void Arrow::setCursorMode( bool _mode )
	{
		if( m_child.empty() == false )
		{
			Node* mainCursor = this->findChildren( Consts::get()->c_Default, false );
			if( mainCursor != NULL )
			{
				_mode ? mainCursor->disable() : mainCursor->enable();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
