#	include "Player.h"
#	include "SoundEmitter.h"

#	include "ParticleSystem.h"

#	include "SceneManager.h"

#	include "Game.h"
#	include "Scene.h"

#	include "Camera2D.h"
#	include "Camera3D.h"

#	include "Arrow.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "ErrorMessage.h"

#	include "Actor.h"

#	include "Amplifier.h"

#	include "Animation.h"

#	include "AnimationObject.h"

#	include "Layer3D.h"

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>
#	include <fstream>

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
		Holder<Player>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
		m_renderCamera2D->destroy();
		m_renderCamera3D->destroy();
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
	//RenderFontInterface * TESTfont;

	Camera3D * cmr3d;

	void Player::init()
	{
		Arrow *defaultArrow = 
			Holder<Game>::hostage()->getDefaultArrow();

		setArrow( defaultArrow );

		Camera2D * cmr = SceneManager::createNodeT<Camera2D>( "Camera2D" );

		mt::vec2f vpSz( 1024, 768 );
		cmr->setPosition( mt::vec2f( 512, 384 ) );
		cmr->setViewportSize( vpSz );

		setRenderCamera2D( cmr );

		cmr3d = SceneManager::createNodeT<Camera3D>( "Camera3D" );
		cmr3d->setAspect(1.3f);
		cmr3d->setFar(1000.0f);
		cmr3d->setNear(0.5f);
		cmr3d->setFov( 3.14f / 4.f );
		cmr3d->setPosition(mt::vec3f(300,0,300));

		cmr3d->lookAt(mt::vec3f(0,0,0));

		setRenderCamera3D(cmr3d);

	//	TESTfont = Holder<RenderEngine>::hostage()->loadFont("russian.xml");

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
	bool Player::handleMouseMove( float _x, float _y, float _whell )
	{
		if( m_scene )
		{
			return m_scene->handleMouseMove( _x, _y, _whell );
		}

		return false;
	}

	void updateCallback(AnimationObject * _obj, const std::string & animationName, float time, void * pUserData)
	{
	}

	void completeCallback(AnimationObject * _obj, const std::string & animationName, void * pUserData)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void Player::update(float _timig)
	{
		if( m_scene )
		{
			m_scene->update( _timig );
		}

		if( m_arrow )
		{
			m_arrow->update( _timig );
		}
		
		InputEngine * inputEng = Holder<InputEngine>::hostage();

		if( inputEng->isKey( DIK_F1, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * mesh = layer->getChildrenT<AnimationObject>("TestMesh");
			mesh->play2Blend("paladin_idle.caf",0.9f,"paladin_walk.caf",0.1f);
		}

		if( inputEng->isKey( DIK_F2, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * mesh = layer->getChildrenT<AnimationObject>("TestMesh");
			mesh->play("paladin_idle.caf");
		}

		if( inputEng->isKey( DIK_F3, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * mesh = layer->getChildrenT<AnimationObject>("TestMesh");
			mesh->setCallback("paladin_walk.caf",0,updateCallback,completeCallback,0);
			mesh->play("paladin_walk.caf");
		}

		if( inputEng->isButton( 0, 1 ) == true )
		{
			cmr3d->yaw( 0.128f );
		}

		if( inputEng->isButton( 1, 1 ) == true )
		{
			cmr3d->yaw( -0.128f );
		}

		if( inputEng->isKey( DIK_A, 1 ) == true )
		{
			mt::vec3f a = cmr3d->getLocalPosition();
			mt::vec3f b = cmr3d->getLocalStrafe();
			a += b * 0.1f;
			cmr3d->setPosition( a );
		}

		if( inputEng->isKey( DIK_D, 1 ) == true )
		{
			mt::vec3f a = cmr3d->getLocalPosition();
			mt::vec3f b = cmr3d->getLocalStrafe();
			a -= b * 0.1f;
			cmr3d->setPosition( a );
		}

		if( inputEng->isKey( DIK_W, 1 ) == true )
		{
			mt::vec3f a = cmr3d->getLocalPosition();
			mt::vec3f b = cmr3d->getLocalDirection();
			a += b * 0.1f;
			cmr3d->setPosition( a );
		}

		if( inputEng->isKey( DIK_S, 1 ) == true )
		{
			mt::vec3f a = cmr3d->getLocalPosition();
			mt::vec3f b = cmr3d->getLocalDirection();
			a -= b * 0.1f;
			cmr3d->setPosition( a );
		}


		//if( inputEng->isButton( MOUSE_LEFT, DI_PRESSED ) == true )
		{
			//Actor * actor = m_scene->getEntityT<Actor>("TestActor");

			//const mt::vec3f &dmv = inputEng->getPosition();
			//actor->moveto(dmv.v2);

/*			Animation * anim = m_scene->getEntityT<Animation>("TestAnimation");
			anim->play();
			
			int u = 0;			

			/*const mt::vec3f &dmv = inputEng->getPosition();

			mt::vec2f pick(dmv.v2);
			VisitorPickHotSpot pickHotSpot( pick );
			pickHotSpot.apply(m_scene);
			HotSpot *hs = pickHotSpot.top();

			if( hs )
			{
				const lua_boost::lua_functor * event = hs->event("LeftMouseClick");
				event->call() % lua_boost::ret<void>();
			}
			*/
		}

//		if( inputEng->isButton(MOUSE_RIGHT,DI_PRESSED) == true )
//		{
//			const mt::vec3f &dmv = inputEng->getPosition();
//	
////			psx = dmv.x;
//		//	psy = dmv.y;
//
////
//		//	SoundEmitter * em = m_scene->getEntityT<SoundEmitter>("TestSoundEmitter");
//		//	em->play();
//
//			//Animation * anim = m_scene->getEntityT<Animation>("TestAnimation");
//			//anim->setAnimState(REWIND);
//			//anim->play();
//
//			//Holder<Amplifier>::hostage()->playList("logoSceneMusic.xml");
//		}

		//if( inputEng->isKey( DIK_D, DI_HELD ) == true )
		//{
		//	mt::vec2f cp = m_renderCamera->getLocalPosition();
		//	cp.x -= 1;
		//	m_renderCamera->setPosition( cp );
		//}

		//if( inputEng->isKey( DIK_A, DI_HELD ) == true )
		//{
		//	mt::vec2f cp = m_renderCamera->getLocalPosition();
		//	cp.x += 1;
		//	m_renderCamera->setPosition( cp );
		//}
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
