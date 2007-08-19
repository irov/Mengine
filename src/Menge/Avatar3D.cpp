#	include "Avatar3D.h"

#	include "ObjectImplement.h"

#	include "ActionMove.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Avatar3D)
	//////////////////////////////////////////////////////////////////////////
	Avatar3D::Avatar3D()
	: m_isMove(false)
	, m_velocity(10)
	, m_destpos(0.0f,0.0f,0.0f)
	, m_dir(0.0f,0.0f,0.0f)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleMouseMove( float _x, float _y, float _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::loader( TiXmlElement * _xml )
	{
		AnimationObject::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::_update( float _timing)
	{
		m_actionScheduler.update( _timing );
		
		AnimationObject::_update(_timing);

		calculateNewPos( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::calculateNewPos( float _timing )
	{
		if( m_isMove )
		{
			float actual_distance = m_velocity * _timing;

			mt::vec3f pos = getLocalPosition();

			float m_distance = mt::length_v3_v3(pos, m_destpos);

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
	void Avatar3D::calculateDestPos(const mt::vec3f& _dest)
	{
		mt::vec3f pos = getLocalPosition();
		mt::vec3f vec = _dest - pos;

		norm_v3(m_dir,vec);
		m_destpos = _dest;
		m_isMove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::moveToPoint(const mt::vec3f& _dest)
	{
		//m_actionScheduler.addAction( new ActionMove(this, mt::vec3f(0,-100,0)) );
		//m_actionScheduler.addAction( new ActionMove(this, mt::vec3f(0,100,0)) );
		//m_actionScheduler.runAction( new ActionMove(this, mt::vec3f(50,50,0)) );
		m_actionScheduler.runAction( new ActionMove(this, _dest) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::stop()
	{
		m_isMove = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::isMoving() const
	{
		return m_isMove;
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::lookTo(const mt::vec3f& _target)
	{
		const mt::vec3f& pos = getLocalPosition();
		m_dir = _target - pos;
		norm_v3(m_dir,m_dir);
	//	setDirection(m_dir);
	}
}