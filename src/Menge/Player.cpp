#	include "Player.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Arrow.h"
#	include "HotSpot.h"

#	include "RenderEngine.h"
#	include "InputEngine.h"
#	include "InputEnum.h"

#	include "ScriptFunction.h"

#	include "VisitorPickHotSpot.h"

#	include "VisitorRender.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_chapter(0)
	, m_scene(0)
	, m_arrow(0)
	{
		Keeper<Player>::keep(this);
	}

	Player::~Player()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Player::setChapter(Chapter *_chapter)
	{
		m_chapter = _chapter;
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
	void Player::update(float _timig)
	{
		m_scene->update( _timig );
		m_arrow->update( _timig );

		InputEngine * inputEng = Keeper<InputEngine>::hostage();

		if( inputEng->IsButton(MOUSE_LEFT,DI_PRESSED) == true )
		{
			int i = inputEng->GetPosition(0);
			int j = inputEng->GetPosition(1);

			mt::vec2f pick(i,j);
			VisitorPickHotSpot pickHotSpot( pick );
			pickHotSpot.apply(m_scene);
			HotSpot *hs = pickHotSpot.top();

			if( hs )
			{
				ScriptFunction *event = hs->event("LeftMouseClick");
				event->callFunctionVoid();
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
