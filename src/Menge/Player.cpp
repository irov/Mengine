#	include "Player.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Arrow.h"
#	include "BackgroundSound.h"
#	include "RenderEngine.h"
#	include "RenderSystemInterface.h"


#	include "VisitorRender.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Playlist pl("test");
	RenderFontInterface* font;

	Player::Player()
	: m_chapter(0)
	, m_scene(0)
	, m_arrow(0)
	, m_backSound(0)
	{
		//***
		m_backSound = new BackgroundSound();
		m_backSound->play(pl);
		font = Keeper<RenderEngine>::hostage()->loadFont("russian.xml");
		//***
		Keeper<Player>::keep(this);
	}
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
	void Player::setArrow(Arrow * _arrow)
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update(float _timig)
	{
		m_scene->update( _timig );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render()
	{
		VisitorRender vr(m_scene);
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::debugRender()
	{
		m_scene->debugRender();
		m_backSound->update();
		Keeper<RenderEngine>::hostage()->renderText(
			mt::vec2f(25,25),font,"werio walfh; a: :FJKQR:JKRM gyuMEGADET    Hergpyu(&)(&%&%)#$&%)($%");

//		m_arrow->debugRender();
	}
}