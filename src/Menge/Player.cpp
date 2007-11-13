#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"
#	include "Scene.h"

#	include "Camera2D.h"
#	include "Camera3D.h"
#	include "Arrow.h"

#	include "InputEngine.h"

#	include "LogEngine.h"


#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera2D(0)
	, m_renderCamera3D(0)
	, m_switchScene(false)
	, m_nextScene(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
		if( m_renderCamera2D )
		{
			delete m_renderCamera2D;
		}

		if( m_renderCamera3D )
		{
			delete m_renderCamera3D;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentScene( const std::string & _name )
	{
		m_nextScene = Holder<Game>::hostage()
			->getScene( _name );

		if( m_nextScene == 0 )
		{
			MENGE_LOG("ERROR: Scene [%s] not have in Game\n", _name.c_str() );
			return;
		}

		m_switchScene = true;
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

		Camera3D * cmr3d = SceneManager::createNodeT<Camera3D>( "Camera3D" );
		
		cmr3d->setAspect( 1.3f );
		cmr3d->setFar( 1000.0f );
		cmr3d->setNear( 0.5f );
		cmr3d->setFov( 3.14f / 4.f );
		cmr3d->setPosition( mt::vec3f(300,300,0) );
		cmr3d->lookAt( mt::vec3f(0,0,0) );

		setRenderCamera3D(cmr3d);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( size_t _key, bool _isDown )
	{
		if( m_scene )
		{
			return m_scene->handleKeyEvent( _key, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool handler = false;

		if( handler == false )
		{
			for each( GlobalMouseHandler * mouseHandler in m_setGlobalMouseHandler )
			{
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

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( int _x, int _y, int _whell )
	{
		bool handler = false;

		if( handler == false )
		{
			for each( GlobalMouseHandler * mouseHandler in m_setGlobalMouseHandler )
			{
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
	void Player::updateChangeScene()
	{
		if( m_switchScene == true )
		{
			m_switchScene = false;

			m_nextScene->activate();

			if( m_scene )
			{
				m_scene->deactivate();
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
	void Player::setRenderCamera3D( Camera3D * _camera)
	{
		m_renderCamera3D = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D * Player::getRenderCamera3D()
	{
		return m_renderCamera3D;
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
	void Player::setCamera3DPosition( const mt::vec3f & _pos )
	{
		if( m_renderCamera3D )
		{
			m_renderCamera3D->setPosition( _pos );
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
