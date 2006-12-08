#	include "Player.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Arrow.h"

#	include "RenderEngine.h"

#	include "VisitorRender.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_chapter(0)
	, m_scene(0)
	, m_arrow(0)
	{}

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
	void Player::setArrow(Arrow * _arrow)
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update(float _timig)
	{
		m_scene->update( _timig );
		m_arrow->update( _timig );
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
