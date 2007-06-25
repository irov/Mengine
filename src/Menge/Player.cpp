#	include "Player.h"
#	include "SoundEmitter.h"

#	include "ParticleSystem.h"

#	include "SceneManager.h"

#	include "Game.h"
#	include "Scene.h"
#	include "Camera2D.h"

#	include "Arrow.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "VisitorPickHotSpot.h"

#	include "VisitorRender.h"

#	include "ErrorMessage.h"

#	include "Actor.h"

#	include "Layer.h"

#	include "Amplifier.h"

#	include "Animation.h"

#	include <dinput.h>
#	include <fstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera(0)
	{
		Holder<Player>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
		m_renderCamera->destroy();
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

	void Player::init()
	{
		Arrow *defaultArrow = 
			Holder<Game>::hostage()->getDefaultArrow();

		setArrow( defaultArrow );

		Camera2D *cmr = SceneManager::createNodeT<Camera2D>( "Camera2D" );

		mt::vec2f vpSz( 1024, 768 );
		cmr->setPosition( mt::vec2f( 512, 384 ) );
		cmr->setViewportSize( vpSz );

		setRenderCamera( cmr );

	//	TESTfont = Holder<RenderEngine>::hostage()->loadFont("russian.xml");

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

		//Holder<RenderEngine>::hostage()->renderText(mt::vec2f(56,56),TESTfont,"dfgdfgdgfgdfgf");

		if( inputEng->isButton(MOUSE_LEFT,DI_PRESSED) == true )
		{
			Actor * actor = m_scene->getEntityT<Actor>("TestActor");

			const mt::vec3f &dmv = inputEng->getPosition();
			actor->moveto(dmv.v2);

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

		if( inputEng->isButton(MOUSE_RIGHT,DI_PRESSED) == true )
		{
			const mt::vec3f &dmv = inputEng->getPosition();
	
//			psx = dmv.x;
		//	psy = dmv.y;

//
		//	SoundEmitter * em = m_scene->getEntityT<SoundEmitter>("TestSoundEmitter");
		//	em->play();

			//Animation * anim = m_scene->getEntityT<Animation>("TestAnimation");
			//anim->setAnimState(REWIND);
			//anim->play();

			//Holder<Amplifier>::hostage()->playList("logoSceneMusic.xml");
		}

		if( inputEng->isKey( DIK_D, DI_HELD ) == true )
		{
			mt::vec2f cp = m_renderCamera->getLocalPosition();
			cp.x -= 1;
			m_renderCamera->setPosition( cp );
		}

		if( inputEng->isKey( DIK_A, DI_HELD ) == true )
		{
			mt::vec2f cp = m_renderCamera->getLocalPosition();
			cp.x += 1;
			m_renderCamera->setPosition( cp );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setRenderCamera( Camera2D * _camera)
	{
		m_renderCamera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D * Player::getRenderCamera()
	{
		return m_renderCamera;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render()
	{
		if( m_renderCamera == 0 )
		{
			return;
		}

		const Viewport &vp = m_renderCamera->getViewport();
		
		if( m_scene )
		{
			SceneManager::renderNode( m_scene, vp );
		}
		
		if( m_arrow )
		{
			Viewport avp;
			avp.begin = mt::vec2f(0,0);
			avp.end = mt::vec2f(1024,768);
			SceneManager::renderNode( m_arrow, avp );
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
