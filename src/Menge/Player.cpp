#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Camera3D.h"

#	include "Arrow.h"

#	include "LogEngine.h"

#	include "MousePickerSystem.h"

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
	bool Player::handleKeyEvent( size_t _key, size_t _char, bool _isDown )
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
	bool Player::handleMouseButtonEvent( size_t _button, bool _isDown )
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

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( int _x, int _y, int _whell )
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
		if( m_switchScene == true )
		{
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update( float _timing )
	{
		updateChangeScene();

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
	void Player::render()
	{
		if( m_scene )
		{
			m_scene->render();
		}
				
		if( m_arrow )
		{
			m_arrow->render();
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::debugRender()
	{
		if( m_scene )
		{
			m_scene->debugRender();
		}		
	}
}
