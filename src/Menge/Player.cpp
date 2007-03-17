#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"
#	include "Chapter.h"
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

//#	include "Region.h"

#	include "lua_boost/lua_functor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_chapter(0)
	, m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera(0)
	{
		Holder<Player>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setChapter( const std::string & _name )
	{
		Game * game =
			Holder<Game>::hostage();

		m_chapter = game->getChapter( _name );

		if( m_chapter == 0 )
		{
			ErrorMessage("ERROR: Chapter [%s] not have in Game", _name.c_str() );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::gotoScene( const std::string & _name )
	{
		Game * game =
			Holder<Game>::hostage();

		if( m_chapter == 0 )
		{
			ErrorMessage("ERROR: Please set valid chapter for Player");
		}

		m_scene = m_chapter->getScene( _name );

		if( m_scene == 0 )
		{
			ErrorMessage("ERROR: Scene [%s] not have in Chapter", _name.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setChapter( Chapter * _chapter )
	{
		m_chapter = _chapter;
	}
	//////////////////////////////////////////////////////////////////////////
	Chapter * Player::getChapter()
	{
		return m_chapter;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setScene(Scene * _scene)
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Player::getScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setArrow(Arrow * _arrow)
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::init()
	{
		Arrow *defaultArrow = 
			Holder<Game>::hostage()->getDefaultArrow();

		setArrow( defaultArrow );
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

		if( inputEng->isButton(MOUSE_LEFT,DI_PRESSED) == true )
		{
			Actor * actor = m_scene->getEntityT<Actor>("TestActor");

			const mt::vec3f &dmv = inputEng->getPosition();
			actor->moveto(dmv.v2);
			
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
		const ViewPort &vp = m_renderCamera->getViewPort();
		
		if( m_scene )
		{
			SceneManager::renderNode( m_scene, vp );
		}
		
		if( m_arrow )
		{
			SceneManager::renderNode( m_arrow, vp );
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
