#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "Layer2D.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity )
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
	: m_moveTo(false)
	, m_moveTime(0)

	, m_rotate(false)
	, m_targetDir(0,0)
	, m_rotateTime(0)

	, m_scale( 1.0f, 1.0f )
	, m_speed( 0.0f, 0.0f )
	, m_acceleration( 0.0f, 0.0f )

	, m_nSpeed( 0.0f, 0.0f )
	, m_acTime( 0.0f )
	, m_accelerateTo( false )

	, m_scaleTo( false )
	, m_scalePoint( 1.0f, 1.0f )
	, m_scaleTime( 0.0f )
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
			m_speed = ( _point - _pos ) / _time;
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

			this->callEvent("MOVE_END", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::scaleTo( float _time, const mt::vec2f& _scale )
	{
		const mt::vec2f & scl = getScale();
		float length = mt::length_v2_v2( scl, _scale );
		if( length > 0.00001f )
		{
			/*if( m_moveTo )
			{
				moveStop();
			}*/
			//m_speed = ( _point - _pos ) / _time;
			m_scalePoint = _scale;
			m_scaleTo = true;
			m_scaleTime = _time;
		}
		else
		{
			if( m_scaleTo )
			{
				scaleStop();
			}

			setScale( _scale );

			m_scaleTo = false;

			this->callEvent("SCALE_END", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::moveStop()
	{
		m_moveTo = false;

		this->callEvent("MOVE_STOP", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::rotateStop()
	{
		m_rotate = false;

		this->callEvent("ROTATE_STOP", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::scaleStop()
	{
		m_scaleTo = false;

		this->callEvent("SCALE_STOP", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::rotateTo( float _time, const mt::vec2f & _point )
	{
		const mt::vec2f & _pos = getLocalPosition();
		//float length = mt::length_v2_v2( _pos, _point );
		m_targetDir = mt::norm_v2( _point - _pos );
		const mt::vec2f& dir = getLocalDirection();
		float length = mt::length_v2_v2( dir, m_targetDir );
		if( length > 0.00001f )
		{
	
			if( m_moveTo )
			{
				rotateStop();
			}

			if( m_rotate )
			{
				rotateStop();
			}
			
			m_rotate = true;
			m_rotateTime = _time;
			m_targetDir = mt::norm_v2( _point - _pos );
			printf( "rotateto: %.4f %.4f %.4f ms\n", m_targetDir.x, m_targetDir.y, _time );
		}
		else
		{
			if( m_moveTo )
			{
				rotateStop();
			}

			setLocalDirection( m_targetDir );

			m_rotate = false;

			this->callEvent("ROTATE_END", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::accelerateTo( float _speed, const mt::vec2f& _point, bool _changeDirection )
	{

		const mt::vec2f & pos = getLocalPosition();	
		float v = m_speed.length();

		if( fabsf( _speed - v ) < 0.0001f )
		{
			moveStop();
			this->callEvent("MOVE_END", "()" );
			return;
		}
		float len = ( _point - pos ).length();
		mt::vec2f dir;
		if( v == 0.0f )
			dir = ( _point - pos ) / len;
		else
			dir = m_speed / v;

		m_moveTime = 2.0f * len / ( v + _speed );
		m_acceleration = dir * ( _speed - v ) / m_moveTime;
		m_movePoint = _point;
		m_moveTo = true;
		m_nSpeed = dir * _speed;
		m_accelerateTo = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::flip( bool _x, bool _y )
	{
		TListChildren::iterator it = m_listChildren.begin();
		for(; it != m_listChildren.end(); it++)
		{
			mt::vec2f pos = (*it)->getLocalPosition();
			mt::vec2f dir = (*it)->getLocalDirection();
			if( _x )
			{
				pos.x = - pos.x;
				(*it)->flip( true );
			}
			if( _y )
			{
				pos.y = - pos.y;
				(*it)->flip( false );
			}
			(*it)->setLocalPosition( pos );
			//(*it)->setLocalDirection( -dir );
			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setScale( const mt::vec2f& _scale )
	{
		TListChildren::iterator it = m_listChildren.begin();
		for(; it != m_listChildren.end(); it++)
		{
			mt::vec2f pos = (*it)->getLocalPosition();
			pos.x = pos.x / m_scale.x * _scale.x;
			pos.y = pos.y / m_scale.y * _scale.y;
			(*it)->setLocalPosition( pos );
			(*it)->setScale( _scale );
		}
		m_scale = _scale;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Entity::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_update( float _timing )
	{
		if( m_moveTo )
		{
			if( m_moveTime < _timing )
			{
				setLocalPosition( m_movePoint );

				m_moveTo = false;
				if( m_accelerateTo )
					m_speed = m_nSpeed;
				m_acceleration.x = 0.0f;
				m_acceleration.y = 0.0f;
				m_accelerateTo = false;

				this->callEvent("MOVE_END", "()" );
			}
			else
			{
				m_moveTime -= _timing;
				
				mt::vec2f way_offset = m_speed * _timing;// + dir * m_acceleration * _timing * _timing * 0.5f;

				//mt::vec2f dir = m_speed / m_speed.length();

				m_speed += m_acceleration * _timing;
				
				mt::vec2f & pos = getLocalPositionModify();

				pos += way_offset;
				//pos = m_startPoint + way_offset;

				changePivot();
			}
		}

		if( m_rotate )
		{
			if( m_rotateTime <= _timing  )
			{
				setLocalDirection( m_targetDir );

				m_rotate = false;

				this->callEvent("ROTATE_END", "()" );
			}
			else
			{
				m_rotateTime -= _timing;

				float t = _timing / m_rotateTime;

				const mt::vec2f & dir = getLocalDirection();

				mt::vec2f curr_dir = mt::slerp_v2_v2( dir, m_targetDir, t );
				//mt::vec2f curr_dir = m_targetDir * t + dir * ( 1.0f - t );
				//curr_dir = mt::norm_v2( curr_dir );
				//printf( "dir: %.4f %.4f\n", curr_dir.x, curr_dir.y );

				setLocalDirection( curr_dir );
			}
		}

		if( m_scaleTo )
		{
			if( m_scaleTime <= _timing  )
			{
				setScale( m_scalePoint );

				m_scaleTo = false;

				this->callEvent("SCALE_END", "()" );
			}
			else
			{
				m_scaleTime -= _timing;

				float t = _timing / m_scaleTime;

				const mt::vec2f & scl = getScale();

				//mt::vec2f curr_dir = mt::slerp_v2_v2( dir, m_targetDir, t );
				mt::vec2f curr_scl = m_scalePoint * t + scl * ( 1.0f - t );

				setScale( curr_scl );
			}
		}

		if( m_layer && m_layer->isScrollable() )
		{
			mt::vec2f& pos = getLocalPositionModify();
			if( pos.x > m_layer->getSize().x )
			{
				pos.x -= m_layer->getSize().x;
			}
			else if( pos.x < 0 )
			{
				pos.x += m_layer->getSize().x;
			}
		}
		this->callEvent("UPDATE", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		bool result = SceneNode2D::_activate();

		this->registerEventMethod("MOVE_END", "onMoveEnd" );
		this->registerEventMethod("MOVE_STOP", "onMoveStop" );
		
		this->registerEventMethod("ROTATE_END", "onRotateEnd" );
		this->registerEventMethod("ROTATE_STOP", "onRotateStop" );

		this->registerEventMethod("SCALE_END", "onScaleEnd" );
		this->registerEventMethod("SCALE_STOP", "onScaleStop" );

		this->registerEventMethod("UPDATE", "onUpdate" );

		this->callMethod("onActivate", "()" );

		//m_oldPos = getLocalPosition();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_deactivate()
	{
		SceneNode2D::_deactivate();

		this->callMethod("onDeactivate", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		bool result = SceneNode2D::_compile();

		this->callMethod("onCompile", "()" );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		SceneNode2D::_release();

		this->callMethod("onRelease", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Entity::getSpeedVec() const
	{
		return m_speed;
	}
	//////////////////////////////////////////////////////////////////////////
	float Entity::getSpeed() const
	{
		return m_speed.length();
	}
}
