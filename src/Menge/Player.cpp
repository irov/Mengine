#	include "Player.h"

#	include "Scene.h"

#	include "Arrow.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "InputEnum.h"

#	include "VisitorPickHotSpot.h"

#	include "VisitorRender.h"

#	include "lua_boost/lua_functor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	{
		Keeper<Player>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{}
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
	void Player::update(float _timig)
	{
		m_scene->update( _timig );
		m_arrow->update( _timig );

		InputEngine * inputEng = Keeper<InputEngine>::hostage();

		if( inputEng->isButton(MOUSE_LEFT,DI_PRESSED) == true )
		{
			const mt::vec3f &dmv = inputEng->getPosition();

			mt::vec2f pick(dmv.v2);
			VisitorPickHotSpot pickHotSpot( pick );
			pickHotSpot.apply(m_scene);
			HotSpot *hs = pickHotSpot.top();

			if( hs )
			{
				const lua_boost::lua_functor * event = hs->event("LeftMouseClick");
				event->call() % lua_boost::ret<void>();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render()
	{
		VisitorRender renderer;
		
		renderer.apply(m_scene);
		renderer.apply(m_arrow);
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::debugRender()
	{
		m_scene->debugRender();
	}
}
