#	include "Actor2D.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "math.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Actor2D)
	//////////////////////////////////////////////////////////////////////////
	Actor2D::Actor2D()
	: m_state(STOP)
	, m_destPos(0.0f, 0.0f)
	, m_destDir(0.0f, 0.0f)
	, m_lookAtTarget(false)
	, m_speed(0.0f)
	, m_maxSpeed(0.0f)
	, m_rotateSpeed(0.0f)
	, m_acceleration(0.0f)
	{
		m_animObject =
			this->createChildrenT<Animation>("Animation");
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

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
		//setDirection(mt::vec3f(1,0,0));	
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::lookTo( const mt::vec2f& _target )
	{
		m_state = ROTATE;
		m_lookAtTarget = true;
		m_destPos = _target;
		_calculateDirection();

		m_animObject->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::moveTo( const mt::vec2f& _target )
	{
		m_state = ROTATE;
		m_lookAtTarget = false;
		m_destPos = _target;
		_calculateDirection();

		m_animObject->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::stop()
	{
		m_state = STOP;

		m_animObject->play();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor2D::isMoving() const
	{
		return (m_state == MOVE) || (m_state == ROTATE);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::_render( const mt::mat3f & _rwm, const Viewport & _viewport )
	{
//		Allocator2D::debugRender();
		SceneNode2D::_render(_rwm, _viewport);

		m_animObject->render(_rwm, _viewport);

		/*	debug moving path */
		#ifdef _DEBUG
			mt::vec2f pos = getLocalPosition();
			RenderEngine * renderEng = Holder<RenderEngine>::hostage();
			renderEng->drawLine2D(pos, m_destPos, 0xFFFFFFAA);
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::_update( float _timing )
	{
		SceneNode2D::_update( _timing );

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
	void  Actor2D::_calculateNewPosition( float _timing )
	{
		float speed = _calculateNewSpeed( _timing );	
		float distance = _calculateDistance();
		float actualDist = speed * _timing;

		if ( actualDist < distance )
		{
			mt::vec2f pos = getLocalPosition();
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
	void  Actor2D::_calculateNewDirection( float _timing )
	{
		mt::vec2f lerpDir = getLocalDirection();

		bool  isComplete = mt::slerp_v2_v2( lerpDir, m_destDir, _timing * m_rotateSpeed, lerpDir );

		setDirection( lerpDir );
		
		if( isComplete )
		{
			m_state = m_lookAtTarget ? STOP : MOVE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Actor2D::_calculateNewSpeed( float t )
	{
		if( m_speed <= m_maxSpeed )
		{
			m_speed += m_acceleration * t;

			if( m_speed > m_maxSpeed )
			{
				m_speed = m_maxSpeed;
			}
		}

		return m_speed;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f &	Actor2D::_calculateDirection()
	{
		norm_safe_v2( m_destDir, m_destPos - getLocalPosition() );

		if( mt::dot_v2_v2( m_destDir, m_destDir ) < 0.00001f )
		{
			m_destDir = getLocalDirection();
		}

		return m_destDir;
	}	
	//////////////////////////////////////////////////////////////////////////
	float Actor2D::_calculateDistance()
	{
		mt::vec2f & pos = getLocalPosition();
		float distance = mt::length_v2_v2( pos, m_destPos );
		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor2D::_activate()
	{		
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		m_animObject->activate();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor2D::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
}