#	include "Actor3D.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "math.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Actor3D)
	//////////////////////////////////////////////////////////////////////////
	Actor3D::Actor3D()
	: m_state(STOP)
	, m_destPos(0.0f, 0.0f, 0.0f)
	, m_destDir(0.0f, 0.0f, 0.0f)
	, m_lookAtTarget(false)
	, m_speed(0.0f)
	, m_maxSpeed(0.0f)
	, m_rotateSpeed(0.0f)
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
				XML_CHECK_VALUE_NODE( "RotateSpeed", "Value", m_rotateSpeed );
			}
		}

		// АХТУНГ!  исправть нафиг этот аллокатор. щас для теста
		setDirection(mt::vec3f(1,0,0));	
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::lookTo( const mt::vec3f& _target )
	{
		m_state = ROTATE;
		m_lookAtTarget = true;
		m_destPos = _target;
		_calculateDirection();

		m_animObject->play("paladin_idle.caf");
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::moveTo( const mt::vec3f& _wayPoint )
	{
		m_state = ROTATE;
		m_lookAtTarget = false;
		m_destPos = _wayPoint;
		_calculateDirection();

		m_animObject->play("paladin_walk.caf");
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::stop()
	{
		m_state = STOP;

		m_animObject->play("paladin_idle.caf");
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor3D::isMoving() const
	{
		return (m_state == MOVE) || (m_state == ROTATE);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
	{
		Allocator3D::debugRender();
		SceneNode3D::_render(_rwm, _camera);
		m_animObject->render(_rwm, _camera);
		//debug moving path
		mt::vec3f pos = getLocalPosition();
		RenderEngine * renderEng = Holder<RenderEngine>::hostage();
		renderEng->drawLine3D(pos, m_destPos, 0xFFFFFFAA);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor3D::_update( float _timing )
	{
		if( m_state == ROTATE )
		{
			_calculateNewDirection( _timing );		
		}

		if( m_state == MOVE )
		{
			_calculateNewPosition( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void  Actor3D::_calculateNewPosition( float _timing )
	{
		float speed = _calculateNewSpeed( _timing );	
		float distance = _calculateDistance();
		float actualDist = speed * _timing;

		if ( actualDist < distance )
		{
			mt::vec3f pos = getLocalPosition();
			pos += m_destDir * actualDist;
			setPosition( pos );
		}
		else
		{
			setPosition( m_destPos );
			stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void  Actor3D::_calculateNewDirection( float _timing )
	{
		mt::vec2f lerpDir = getLocalDirection().v2;

		bool  isComplete = mt::slerp_v2_v2( lerpDir, m_destDir.v2, _timing * m_rotateSpeed, lerpDir );

		setDirection( mt::vec3f( lerpDir, 0) );
		
		if( isComplete )
		{
			m_state = m_lookAtTarget ? STOP : MOVE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Actor3D::_calculateNewSpeed( float t )
	{
		if( m_speed <= m_maxSpeed )
		{
			m_speed += m_acceleration * t;

			if(m_speed > m_maxSpeed)
			{
				m_speed = m_maxSpeed;
			}
		}

		return m_speed;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f &	Actor3D::_calculateDirection()
	{
		norm_safe_v3( m_destDir, m_destPos - getLocalPosition() );

		if(mt::dot_v3_v3( m_destDir, m_destDir ) < 0.00001f)
		{
			m_destDir = getLocalDirection();
		}

		return m_destDir;
	}	
	//////////////////////////////////////////////////////////////////////////
	float Actor3D::_calculateDistance()
	{
		mt::vec3f & pos = getLocalPosition();
		float distance = mt::length_v3_v3(pos, m_destPos);
		return distance;
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
}