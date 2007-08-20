#	include "Actor3D.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Actor3D)
	//////////////////////////////////////////////////////////////////////////
	Actor3D::Actor3D()
	: m_isMove(false)
	, m_destpos(0.0f,0.0f,0.0f)
	, m_dir(0.0f,0.0f,0.0f)
	, m_speed(0.0f)
	, m_maxSpeed(0.0f)
	, m_acceleration(0.0f)
	{
		m_animObject =
			this->createChildrenT<AnimationObject>("AnimationObject");
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::loader( TiXmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Animation")
			{
				m_animObject->loader( XML_CURRENT_NODE );
			}

			XML_CHECK_NODE_FOR_EACH("MovementParams")
			{
				XML_CHECK_VALUE_NODE( "MaxSpeed", "Value", m_maxSpeed );
				XML_CHECK_VALUE_NODE( "Acceleration", "Value", m_acceleration );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::_update( float _timing )
	{
		if( m_isMove )
		{
			if( m_speed <= m_maxSpeed )
			{
				m_speed += m_acceleration * _timing;

				if(m_speed > m_maxSpeed)
				{
					m_speed = m_maxSpeed;
				}
			}

			float actual_distance = m_speed * _timing;

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
	void Actor3D::moveToWayPoint( const mt::vec3f& _wayPoint )
	{
		mt::vec3f pos = getLocalPosition();
		mt::vec3f vec = _wayPoint - pos;

		norm_v3(m_dir,vec);
		m_destpos = _wayPoint;

		float m_distance = mt::length_v3_v3(pos, m_destpos);

		m_isMove = true;
//test
		m_animObject->play("paladin_walk.caf");
//
//		m_animObject->executeAction("paladin_walk.caf",0,1,1,false);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::stop()
	{
//test
		m_animObject->play("paladin_idle.caf");
//
		m_isMove = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor3D::isMoving() const
	{
		return m_isMove;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::lookTo(const mt::vec3f& _target)
	{
		const mt::vec3f& pos = getLocalPosition();
		m_dir = _target - pos;
		norm_v3(m_dir,m_dir);
		setDirection(m_dir);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor3D::_activate()
	{		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
	{
		SceneNode3D::_render(_rwm, _camera);
		m_animObject->render(_rwm, _camera);
	}
}