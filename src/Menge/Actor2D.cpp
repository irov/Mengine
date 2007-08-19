#	include "Actor2D.h"

#	include "ObjectImplement.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Actor2D)
	//////////////////////////////////////////////////////////////////////////
	Actor2D::Actor2D()
	: m_isMove(false)
	, m_velocity(1.0f)
	, m_destpos(0.0f,0.0f)
	, m_dir(0.0f,0.0f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::_update( float _timing )
	{
		if( m_isMove )
		{
			float actual_distance = m_velocity * _timing;

			mt::vec2f pos = getLocalPosition();

			float m_distance = mt::length_v2_v2(pos, m_destpos);

			//setDirection(m_dir);

			if (actual_distance > m_distance)
			{
				m_isMove = false;
				setPosition(m_destpos);
			}
			else
			{
				pos += m_dir * actual_distance;
				setPosition(pos);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::moveToWayPoint( const mt::vec2f& _wayPoint )
	{
		mt::vec2f pos = getLocalPosition();
		mt::vec2f vec = _wayPoint - pos;

		norm_v2(m_dir,vec);
		m_destpos = _wayPoint;
		m_isMove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::stop()
	{
		m_isMove = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor2D::isMoving() const
	{
		return m_isMove;
	}
	//////////////////////////////////////////////////////////////////////////
}