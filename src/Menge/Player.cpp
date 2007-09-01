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

#	include "Actor2D.h"

#	include "Amplifier.h"

#	include "Animation.h"

#	include "AnimationObject.h"

#	include "Layer2D.h"

#	include "Layer3D.h"

#	include "AnimationBone.h"

#	include "Avatar3D.h"

#	include "math/box2.h"

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
		if( m_renderCamera2D )
		{
			m_renderCamera2D->destroy();
		}

		if( m_renderCamera3D )
		{
			m_renderCamera3D->destroy();
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
		cmr3d->setPosition(mt::vec3f(300,300,0));

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
	{}

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

		if( inputEng->isButton( 0, 1 ) == true )
		{
			mt::vec2f pos = inputEng->getPosition().v2;

			mt::vec3f camDir = cmr3d->getDirectionFromMouse(pos.x,pos.y);
			mt::vec3f camPos = cmr3d->getWorldPosition();
			
			mt::planef	plane(mt::vec3f(0,1,1),0);

			float t = (plane.d - dot_v3_v3(plane.norm,camPos))/dot_v3_v3(plane.norm,camDir);

			printf("%f  \n", t);
			if(t >= 0.0f && t <= 1.0f)
			{
				printf("moving  \n");
				mt::vec3f q = camPos + t * camDir;

				Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
				Avatar3D * avat = layer->getChildrenT<Avatar3D>("TestAvatar");
				avat->moveToPoint(q);

				printf("%f %f %f \n", q.x,q.y,q.z);
			}
		//	cmr3d->yaw( 0.128f );
		}

		if( inputEng->isButton( 1, 1 ) == true )
		{
		//	cmr3d->yaw( -0.128f );
		}


		if( inputEng->isKey( DIK_1, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			Avatar3D * avat = layer->getChildrenT<Avatar3D>("TestAvatar");
			avat->moveToPoint(mt::vec3f(50,0,10));
		}

		if( inputEng->isKey( DIK_3, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			Avatar3D * avat = layer->getChildrenT<Avatar3D>("TestAvatar");
			avat->lookAtPoint(mt::vec3f(0,0,100));
		}

		if( inputEng->isKey( DIK_2, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			Avatar3D * avat = layer->getChildrenT<Avatar3D>("TestAvatar");
			avat->moveToPoint(mt::vec3f(0,0,-50));
		//	//anim->play2Blend("paladin_idle.caf",0.4f,"paladin_walk.caf",0.6f);
		}

		if( inputEng->isKey( DIK_4, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			Avatar3D * avat = layer->getChildrenT<Avatar3D>("TestAvatar");

			std::vector<mt::vec3f> points;
			points.push_back(mt::vec3f(0,10,0));
			points.push_back(mt::vec3f(50,10,0));
			points.push_back(mt::vec3f(10,100,0));
			points.push_back(mt::vec3f(-10,200,0));
			avat->moveByWayPoints(points);
		}



		if( inputEng->isKey( DIK_F1, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * anim = layer->getChildrenT<AnimationObject>("TestMesh");
			anim->play2Blend("paladin_idle.caf",0.4f,"paladin_walk.caf",0.6f);
		}

		if( inputEng->isKey( DIK_F2, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * anim = layer->getChildrenT<AnimationObject>("TestMesh");
			anim->play("paladin_idle.caf");
		}

		if( inputEng->isKey( DIK_F3, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * anim = layer->getChildrenT<AnimationObject>("TestMesh");
			anim->setCallback("paladin_walk.caf",0,updateCallback,completeCallback,0);
			anim->play("paladin_walk.caf");
		}

		if( inputEng->isKey( DIK_F4, 1 ) == true )
		{
			Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * anim = layer->getChildrenT<AnimationObject>("TestMesh");
			anim->executeAction("paladin_walk.caf",0,1,1,false);



		/*	Layer3D * layer = m_scene->getChildrenT<Layer3D>("Back");
			AnimationObject * anim = layer->getChildrenT<AnimationObject>("TestMesh");
			static mt::vec3f pos(0,0,0);
			anim->setPosition(pos);
			pos.y+=7.5f;
			*/
		}

		if( inputEng->isKey( DIK_F5, 1 ) == true )
		{
			Layer2D * layer = m_scene->getChildrenT<Layer2D>("Main");
			Actor2D * act = layer->getChildrenT<Actor2D>("TestActor");
			//Sprite * spr = act->getChildrenT<Sprite>("TestSprite");
			//spr->setImageIndex(1);
		}

	/*	if( inputEng->isButton( 0, 1 ) == true )
		{
			cmr3d->yaw( 0.128f );
		}

		if( inputEng->isButton( 1, 1 ) == true )
		{
			cmr3d->yaw( -0.128f );
		}
*/
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

		if( inputEng->isKey( DIK_F9, 1 ) == true )
		{
			Holder<Amplifier>::hostage()->playList("logoSceneMusic.xml");
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
