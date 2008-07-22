#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"

//#	include "Camera3D.h"

#	include "Arrow.h"

#	include "LogEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera2D(0)
	, m_switchScene(false)
	, m_nextScene(0)
	, m_destroyOldScene( false )
	, m_restartScene( false )
	, m_arrowHided( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
		if( m_renderCamera2D )
		{
			delete m_renderCamera2D;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentScene( const std::string & _name, bool _destroyOld )
	{
		m_nextScene = Holder<Game>::hostage()
			->getScene( _name );

		if( m_nextScene == 0 )
		{
			MENGE_LOG("ERROR: Scene [%s] not have in Game\n", _name.c_str() );
			return;
		}

		if( m_nextScene == m_scene )
		{
			m_restartScene = true;
		}
		m_switchScene = true;
		m_destroyOldScene = _destroyOld;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Player::getCurrentScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setArrow(Arrow * _arrow)
	{
		if( m_arrow )
		{
			m_arrow->deactivate();
		}

		m_arrow = _arrow;
		
		if( m_arrow )
		{
			m_arrow->activate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Player::getArrow()
	{
		return m_arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Player::getPositionClick() const
	{
		return m_arrow->getWorldPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::init()
	{
		Arrow * defaultArrow = 
			Holder<Game>::hostage()->getDefaultArrow();

		setArrow( defaultArrow );

		Camera2D * cmr = SceneManager::createNodeT<Camera2D>( "Camera2D" );

		mt::vec2f vpSz( 1024, 768 );
		cmr->setLocalPosition( mt::vec2f( 512, 384 ) );
		cmr->setViewportSize( vpSz );

		setRenderCamera2D( cmr );

	/*	Camera3D * cmr3d = SceneManager::createNodeT<Camera3D>( "Camera3D" );
		
		cmr3d->setAspect( 1.3f );
		cmr3d->setFar( 1000.0f );
		cmr3d->setNear( 0.5f );
		cmr3d->setPosition( mt::vec3f(100,100,0) );
		cmr3d->lookAt( mt::vec3f(0,0,0) );

		setRenderCamera3D( cmr3d );
		*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalKeyHandler::iterator
				it = m_setGlobalKeyHandler.begin(),
				it_end = m_setGlobalKeyHandler.end();
			it != it_end;
			++it)
			{
				GlobalKeyHandler * keyHandler = *it;
				if( handler = keyHandler->handleGlobalKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleKeyEvent( _key, _char, _isDown );
			}
		}

		if( handler == false )
		{
			handler = Holder<MousePickerSystem>::hostage()
				->handleKeyEvent( _key, _char, _isDown );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			++it)
			{
				GlobalMouseHandler * mouseHandler = *it;
				if( handler = mouseHandler->handleGlobalMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleMouseButtonEvent( _button, _isDown );
			}
		}

		if( handler == false )
		{
			handler = Holder<MousePickerSystem>::hostage()
				->handleMouseButtonEvent( _button, _isDown );
		}

		if( m_scene )
		{
			m_scene->handleMouseButtonEventEnd( _button, _isDown );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( float _x, float _y, int _whell )
	{
		m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			++it)
			{
				GlobalMouseHandler * mouseHandler = *it;
				if( handler = mouseHandler->handleGlobalMouseMove( _x, _y, _whell ) )
				{
					break;
				}
			}
		}


		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleMouseMove( _x, _y, _whell );
			}
		}

		if( handler == false )
		{
			handler = Holder<MousePickerSystem>::hostage()
				->handleMouseMove( _x, _y, _whell );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::regGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = m_setGlobalMouseHandler.find( _handler );
		if( it_find == m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.insert( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = m_setGlobalMouseHandler.find( _handler );
		if( it_find != m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.erase( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = m_setGlobalKeyHandler.find( _handler );
		if( it_find == m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.insert( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = m_setGlobalKeyHandler.find( _handler );
		if( it_find != m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.erase( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateChangeScene()
	{
		if( m_restartScene )		// just restart scene
		{
			std::string name = m_scene->getName();
			m_scene->deactivate();
			m_scene->release();
			if( m_destroyOldScene )
			{
				Holder<MousePickerSystem>::hostage()->reset();
				Holder<Game>::hostage()->destroyScene( m_scene->getName() );
			}
			m_scene = Holder<Game>::hostage()->getScene( name );
			m_scene->compile();
			m_scene->activate();

			m_restartScene = false;
			m_switchScene = false;
		}
		else if( m_switchScene == true )
		{
			m_renderCamera2D->deactivate();

			m_switchScene = false;

			m_nextScene->compile();
			m_nextScene->activate();

			if( m_scene )
			{
				m_scene->deactivate();
				m_scene->release();
				if( m_destroyOldScene )
				{
					Holder<MousePickerSystem>::hostage()->reset();
					Holder<Game>::hostage()->destroyScene( m_scene->getName() );
				}
			}
			m_scene = m_nextScene;


			//m_scene->activate();
			m_renderCamera2D->activate();
			//Holder<ResourceManager>::hostage()->_dumpResources();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update( float _timing )
	{
		updateChangeScene();

		if( m_renderCamera2D )
		{
			m_renderCamera2D->update( _timing );
		}

		if( m_scene )
		{
			m_scene->update( _timing );
		}

		if( m_arrow )
		{
			m_arrow->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setRenderCamera2D( Camera2D * _camera)
	{
		m_renderCamera2D = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D * Player::getRenderCamera2D()
	{
		return m_renderCamera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCamera2DPosition( const mt::vec2f & _pos )
	{
		if( m_renderCamera2D )
		{
			m_renderCamera2D->setLocalPosition( _pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render( bool _enableDebug )
	{
		if( m_scene )
		{
			const Viewport & viewport = 
				m_renderCamera2D->getViewport();

			m_scene->render( viewport, _enableDebug );
		}

		if( m_arrow )
		{
			Holder<RenderEngine>::hostage()
				->beginLayer2D();

			Viewport viewport;
			viewport.begin = mt::vec2f( 0.f, 0.f );
			viewport.end = mt::vec2f( 1024.f, 768.f );

			//Holder<RenderEngine>::hostage()
			//	->setRenderViewport( viewport );

			m_arrow->render( viewport, _enableDebug );

			Holder<RenderEngine>::hostage()
				->endLayer2D();
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onMouseLeave()
	{
		m_arrow->onMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onMouseEnter()
	{
		m_arrow->onMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
}