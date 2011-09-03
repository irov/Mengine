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
		, m_invalidateClickMatrix(true)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_offsetClick = _offsetClick;

		this->invalidateClickMatrix_();
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

		const mt::vec2f & pos = 
			InputEngine::get()->getCursorPosition();

		this->setLocalPosition( pos );

		//const mt::vec2f & mp = 
		//	InputEngine::get()->getCursorPosition();

		////float vpdx = 1.0f;
		////float vpdy = 1.0f;

		////float dx = 0.0f;
		////float dy = 0.0f;

		////Game * game = Game::get();
		////RenderEngine * renderEngine = RenderEngine::get();

		//const Resolution& contentResolution = Game::get()->getContentResolution();
		//const Resolution& currentResolution = Application::get()->getCurrentResolution();


		////printf("%d %d\n", currentResolution.getWidth(), currentResolution.getHeight() );
		////printf("%f %f\n", mp.x, mp.y );
		////	//const Viewport & viewport = renderEngine->getRenderViewport();

		//mt::vec2f vpd;
		//vpd.x = float( contentResolution.getWidth() ) / float( currentResolution.getWidth() );
		//vpd.y = float( contentResolution.getHeight() ) / float( currentResolution.getHeight() );
		////dx = -viewport.begin.x;
		////dy = -viewport.begin.y;

		//mt::vec2f nmp;
		//nmp.x = vpd.x * mp.x;
		//nmp.y = vpd.y * mp.y;

		////this->setLocalPosition( mt::vec2f(fx, fy) );

		//this->setLocalPosition( nmp );
		////this->setLocalPosition( mp );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		bool cursorMode = 
			Application::get()->getCursorMode();

		setCursorMode( cursorMode );

		std::size_t num_points = boost::geometry::num_points(m_polygon);
		if( num_points == 0 )
		{
			boost::geometry::append( m_polygon, mt::vec2f(0.f,0.f) );
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
			//BIN_CASE_ATTRIBUTE_METHOD( Protocol::Polygon_Point, &Arrow::addPoint_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::addPoint_( const mt::vec2f & _v )
	{
		boost::geometry::append( m_polygon, _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setPolygon( const Polygon & _polygon )
	{
		m_polygon = _polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & Arrow::getPolygon() const
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseLeave()
	{
		Node::hide( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseEnter()
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
	void Arrow::setContentResolution( const Resolution & _resolution )
	{
		m_contentResolution = _resolution;

		this->invalidateClickMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setCurrentResolution( const Resolution & _resolution )
	{
		m_currentResolution = _resolution;

		this->invalidateClickMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setCursorMode( bool _mode )
	{
		if( m_child.empty() )
		{
			return;
		}

		Node* mainCursor = this->findChildren( Consts::get()->c_Default, false );

		if( mainCursor == NULL )
		{
			return;
		}
		
		if( _mode )
		{
			mainCursor->disable();
		}
		else
		{
			mainCursor->enable();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Arrow::getClickMatrix()
	{
		if( m_invalidateClickMatrix == true )
		{
			this->updateClickMatrix_();
		}

		return m_clickMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getClickPosition()
	{
		const mt::mat3f & cm = this->getClickMatrix();

		return cm.v2.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::updateClickMatrix_()
	{
		m_invalidateClickMatrix = false;

		mt::ident_m3( m_clickMatrix );
		mt::translate_m3( m_clickMatrix, m_clickMatrix, m_offsetClick );

		mt::vec2f resolutionScale = m_contentResolution.getScale( m_currentResolution );

		mt::vec3f scale;
		scale.x = resolutionScale.x;
		scale.y = resolutionScale.y;
		scale.z = 1.f;

		MENGE_LOG_ERROR("resolutionScale %f %f", scale.x, scale.y);

		mt::scale_m3( m_clickMatrix, scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::invalidateClickMatrix_()
	{
		m_invalidateClickMatrix = true;
	}
}
