#	include "Actor.h"

#	include "ObjectImplement.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Actor)
//////////////////////////////////////////////////////////////////////////
Actor::Actor()
: m_isMove(false)
, m_speedMove(5.0f)
, m_destpos(0.0f,0.0f)
, m_dir(0.0f,0.0f)
{}
//////////////////////////////////////////////////////////////////////////
void Actor::loader( TiXmlElement * _xml )
{
	SceneNode2D::loader(_xml);
}
//////////////////////////////////////////////////////////////////////////
void Actor::_update(float _timing)
{
	if( m_isMove )
	{
		float t = m_speedMove * _timing;

		const mt::vec2f & pos = getLocalPosition();

		float m_distance = mt::length_v2_v2(pos, m_destpos);

		if (t > m_distance)
		{
			m_isMove = false;
			setDirection(m_dir);
			setPosition(m_destpos);
		}

		if (m_isMove == true)
		{
			mt::vec2f pos = getLocalPosition();
			pos += m_dir * t;
			setDirection(m_dir);
			setPosition(pos);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void Actor::moveto(const mt::vec2f& _destpos)
{
	mt::vec2f pos = getLocalPosition();
	mt::vec2f vec = _destpos - pos;

	norm_v2(m_dir,vec);
	m_destpos = _destpos;
	m_isMove = true;
}