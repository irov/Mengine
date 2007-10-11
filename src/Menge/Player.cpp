#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Camera3D.h"

#	include "Arrow.h"

#	include "InputEngine.h"

#	include "ErrorMessage.h"

#	include "Layer3D.h"

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
		Scene * scene = Holder<Game>::hostage()
			->getScene( _name );

		if( scene == 0 )
		{
			ErrorMessage("ERROR: Scene [%s] not have in Game", _name.c_str() );
		}

		if( m_scene )
		{
			m_scene->deactivate();
		}

		m_scene = scene;
		m_scene->activate();
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
		if( m_scene )
		{
			return m_scene->handleMouseButtonEvent( _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( int _x, int _y, int _whell )
	{
		if( m_scene )
		{
			return m_scene->handleMouseMove( _x, _y, _whell );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update( size_t _timing )
	{
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
			Viewport avp;
			avp.begin = mt::vec2f(0,0);
			avp.end = mt::vec2f(1024,768);
			
			const mt::mat3f &wm = m_arrow->getWorldMatrix();
			m_arrow->render( wm, avp );
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
