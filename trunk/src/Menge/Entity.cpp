#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "Layer2D.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Entity)
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
	: m_moveTo(false)
	, m_moveTime(0)

	//, m_scale( 1.0f, 1.0f )
	, m_velocity( 0.0f, 0.0f )
	, m_acceleration( 0.0f, 0.0f )

	, m_nSpeed( 0.0f, 0.0f )
	, m_acTime( 0.0f )
	, m_accelerateTo( false )

	, m_physicController( false )

	, m_onUpdateEvent( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity::~Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::moveTo( float _time, const mt::vec2f & _point, bool _changeDirection )
	{
		const mt::vec2f & _pos = getLocalPosition();
		float length = mt::length_v2_v2( _pos, _point );
		if( length > 0.00001f && _time > 0.00001f )
		{
			if( m_moveTo )
			{
				moveStop();
			}
			m_velocity = ( _point - _pos ) / _time;
			m_movePoint = _point;
			if( _changeDirection )
			{
				rotateStop();
				mt::vec2f dir = mt::norm_v2( _point - _pos );
				setLocalDirection( dir );
			}
			m_moveTo = true;
			m_moveTime = _time;

			m_acceleration.x = 0.0f;
			m_acceleration.y = 0.0f;
		}
		else
		{
			if( m_moveTo )
			{
				moveStop();
			}

			setLocalPosition( _point );

			m_moveTo = false;

			this->callEvent( EVENT_MOVE_END, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::scaleTo( float _time, const mt::vec2f& _scale )
	{
		const mt::vec2f & scl = getScale();
		if( m_scaleTo.start( scl, _scale, _time, mt::length_v2 ) == false )
		{
			if( m_scaleTo.isStarted() )
			{
				scaleStop();
			}
			setScale( _scale );
			this->callEvent( EVENT_SCALE_END, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::moveStop()
	{
		m_moveTo = false;

		this->callEvent( EVENT_MOVE_STOP, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::rotateStop()
	{
		m_rotateTo.stop();

		this->callEvent( EVENT_ROTATE_STOP, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::scaleStop()
	{
		m_scaleTo.stop();

		this->callEvent( EVENT_SCALE_STOP, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::rotateTo( float _time, const mt::vec2f & _point )
	{
		const mt::vec2f& dir = getLocalDirection();
		const mt::vec2f& pos = getLocalPosition();
		mt::vec2f newDir = _point - pos;

		if( newDir.length() < 0.0001f )
		{
			this->callEvent( EVENT_ROTATE_END, "()" );
			return;
		}
		float cos_alpha = mt::dot_v2_v2( dir, newDir ) / newDir.length();
		float sin_alpha = dir.x * newDir.y - dir.y * newDir.x;
		if( cos_alpha > 1.0f )
		{
			cos_alpha = 1.0f;
		}
		else if( cos_alpha < -1.0f )
		{
			cos_alpha = -1.0f;
		}
		float alpha = ::acosf( cos_alpha );
		if( sin_alpha < 0.0f )
		{
			alpha = -alpha;
		}

		if( m_rotateTo.start( 0.0f, alpha, _time, ::fabsf ) == false )
		{
			setAngle( getAngle() + alpha );
			this->callEvent( EVENT_ROTATE_END, "()" );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::accelerateTo( float _speed, const mt::vec2f& _point, bool _changeDirection )
	{

		const mt::vec2f & pos = getLocalPosition();	

		float len = ( _point - pos ).length();
		mt::vec2f dir;
		dir = ( _point - pos ) / len;
		m_nSpeed = dir * _speed;

		float s = mt::dot_v2_v2( m_velocity, dir );
		m_velocity = dir * s;

		float v = mt::length_v2_v2( m_velocity, m_nSpeed );

		if( v < 0.0001f )
		{
			moveStop();
			this->callEvent( EVENT_MOVE_END, "()" );
			return;
		}
		//float len = ( _point - pos ).length();
		//mt::vec2f dir;
		//if( v == 0.0f )
		//	dir = ( _point - pos ) / len;
		//else
		//	dir = m_speed / v;
		if( _changeDirection )
		{
			rotateStop();
			mt::vec2f dir = mt::norm_v2( _point - pos );
			setLocalDirection( dir );
		}

		//m_moveTime = 2.0f * len / ( v + _speed );
		//m_acceleration = dir * ( _speed - v ) / m_moveTime;
		//m_moveTime = 2.0f * ( _point.x - pos.x ) / ( m_nSpeed.x + m_speed.x );
		m_moveTime = 2.0f * len / ( m_nSpeed.length() + s );
		//m_acceleration.x = ( m_nSpeed.x - m_speed.x ) / m_moveTime;
		//m_acceleration.y = ( m_nSpeed.y - m_speed.y ) / m_moveTime;
		m_acceleration = ( m_nSpeed - m_velocity ) / m_moveTime;
		m_movePoint = _point;
		m_moveTo = true;
		m_accelerateTo = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::flip( bool _x, bool _y )
	{
		if( _x )
		{
			setScale( mt::vec2f( -m_scale.x, m_scale.y ) );
		}
		if( _y )
		{
			setScale( mt::vec2f( m_scale.x, -m_scale.y ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_update( float _timing )
	{
		Node::_update( _timing );
		if( m_physicController )
		{
			if( m_velocity != mt::vec2f::zero_v2 )
			{
				const float* ppos = m_interface->getPosition();
				mt::vec2f pos( ppos[0], ppos[1] );
				pos += m_velocity * _timing;
				setLocalPosition( pos );
			}

			if( m_rotateTo.isStarted() )
			{
				float angle;
				m_rotateTo.update( _timing, &angle );
				setAngle( getAngle() + m_rotateTo.getDelta() );
				if( m_rotateTo.isStarted() == false )
				{
					this->callEvent( EVENT_ROTATE_END, "()" );
				}
			}

			if( m_scaleTo.isStarted() )
			{
				mt::vec2f scale;
				m_scaleTo.update( _timing, &scale );
				setScale( scale );
				if( m_scaleTo.isStarted() == false )
				{
					this->callEvent( EVENT_SCALE_END, "()" );
				}
			}

			RigidBody2D::_update( _timing );

			//this->callEvent("UPDATE", "(f)", _timing );
			return;
		}

		if( m_moveTo )
		{
			if( m_moveTime < _timing )
			{
				setLocalPosition( m_movePoint );

				m_moveTo = false;
				if( m_accelerateTo )
					m_velocity = m_nSpeed;
				m_acceleration.x = 0.0f;
				m_acceleration.y = 0.0f;
				m_accelerateTo = false;

				this->callEvent( EVENT_MOVE_END, "()" );
			}
			else
			{
				m_moveTime -= _timing;
				
				mt::vec2f way_offset = m_velocity * _timing;// + dir * m_acceleration * _timing * _timing * 0.5f;


				m_velocity += m_acceleration * _timing;
				
				translate( way_offset );
			}
		}

		if( m_rotateTo.isStarted() )
		{
			float angle;
			m_rotateTo.update( _timing, &angle );
			setAngle( getAngle() + m_rotateTo.getDelta() );
			if( m_rotateTo.isStarted() == false )
			{
				this->callEvent( EVENT_ROTATE_END, "()" );
			}
		}

		if( m_scaleTo.isStarted() )
		{
			mt::vec2f scale;
			m_scaleTo.update( _timing, &scale );
			setScale( scale );
			if( m_scaleTo.isStarted() == false )
			{
				this->callEvent( EVENT_SCALE_END, "()" );
			}
		}

		if( m_onUpdateEvent )
		{
			this->callEvent( EVENT_UPDATE, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		bool result = RigidBody2D::_activate();

		this->registerEvent( EVENT_MOVE_END, ("onMoveEnd") );
		this->registerEvent( EVENT_MOVE_STOP, ("onMoveStop") );
		
		this->registerEvent( EVENT_ROTATE_END, ("onRotateEnd") );
		this->registerEvent( EVENT_ROTATE_STOP, ("onRotateStop") );

		this->registerEvent( EVENT_SCALE_END, ("onScaleEnd") );
		this->registerEvent( EVENT_SCALE_STOP, ("onScaleStop") );

		if( m_onUpdateEvent == false )
		{
			m_onUpdateEvent = this->registerEvent( EVENT_UPDATE, ("onUpdate") );
		}

		this->registerEvent( EVENT_COLLIDE, ("onCollide") );

		this->callMethod( ("onActivate"), "()" );

		//m_oldPos = getLocalPosition();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_deactivate()
	{
		RigidBody2D::_deactivate();

		this->callMethod( ("onDeactivate"), "()" );

	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		bool result = Node::_compile();

		this->callMethod( ("onCompile"), "()" );

		if( m_physicController )
		{
			RigidBody2D::_compile();
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		Node::_release();

		if( m_physicController )
		{
			RigidBody2D::_release();
		}

		this->callMethod( ("onRelease"), "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setVelocity( const mt::vec2f & _velocity )
	{
		m_velocity = _velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Entity::getVelocity() const
	{
		return m_velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	float Entity::getVelocitySpeed() const
	{
		return m_velocity.length();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::loader( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "PhysicBody" )
			{
				XML_PARSE_ELEMENT( this, &RigidBody2D::_loaderPhysics );
				m_physicController = true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		RigidBody2D * other = static_cast<RigidBody2D*>( _otherObj->getUserData() );
		this->callEvent( EVENT_COLLIDE, "(OOffff)", RigidBody2D::getEmbedding(), other->getEmbedding(), _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::directTo( float _time, const mt::vec2f& _dir )
	{
		const mt::vec2f & _pos = getLocalPosition();
		rotateTo( _time, _pos + _dir );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::angleTo( float _time, float _angle )
	{
		const mt::vec2f& dir = getLocalDirection();
		const mt::vec2f& pos = getLocalPosition();
		float alpha = ::acosf( dir.x );
		if( dir.y < 0.0f ) alpha = -alpha;

		if( m_rotateTo.start( alpha, alpha + _angle, _time, ::fabsf ) == false )
		{
			if( m_rotateTo.isStarted() )
			{
				rotateStop();
			}
			setAngle( alpha + _angle );
			this->callEvent( EVENT_ROTATE_END, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_render( Camera2D * _camera )
	{

	}
	//////////////////////////////////////////////////////////////////////////
}
