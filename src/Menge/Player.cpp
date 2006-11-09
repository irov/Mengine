#	include "Player.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Arrow.h"

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
	//////////////////////////////////////////////////////////////////////////
	void Player::setChapter( Chapter *_chapter)
	{
		m_chapter = _chapter;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setScene( Scene * _scene )
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update( float _timig )
	{
		m_scene->update( _timig );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render()
	{
		VisitorRender vr(m_scene);
		m_scene->debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::debugRender()
	{
		m_scene->debugRender();
	}
}