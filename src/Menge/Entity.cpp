#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity )
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
	: m_moveTo(false)
	, m_moveTime(0)
	, m_moveSpeed(0.f)

	, m_rotate(false)
	, m_targetDir(0,0)
	, m_rotateTime(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity::~Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::moveTo( size_t _time, const mt::vec2f & _point, bool _changeDirection )
	{
		const mt::vec2f & _pos = getLocalPosition();
		float length = mt::length_v2_v2( _pos, _point );
		if( length > 0.00001f )
		{
			if( m_moveTo )
			{
				moveStop();
			}

			if( m_rotate )
			{
				rotateStop();
			}
			
			m_movePoint = _point;
			m_moveTime = _time;
			m_moveTo = true;

			m_moveSpeed = length / float(_time);

			m_moveDir = mt::norm_v2( _point - _pos );
			if( _changeDirection )
			{
				setLocalDirection( m_moveDir );
			}
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
	void Entity::rotateTo( size_t _time, const mt::vec2f & _point )
	{
		const mt::vec2f & _pos = getLocalPosition();
		float length = mt::length_v2_v2( _pos, _point );
		if( length > 0.00001f )
		{
	
			if( m_moveTo )
			{
				moveStop();
			}

			if( m_rotate )
			{
				rotateStop();
			}
			
			m_rotate = true;
			m_rotateTime = _time;
			m_targetDir = mt::norm_v2( _point - _pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_update( size_t _timing )
	{
		if( m_moveTo )
		{
			if( m_moveTime < _timing )
			{
				setLocalPosition( m_movePoint );

				m_moveTo = false;

				this->callEvent("MOVE_END", "()" );
			}
			else
			{
				m_moveTime -= _timing;

				float way_length = m_moveSpeed * float(_timing);
				
				mt::vec2f way_offset = m_moveDir * way_length;
				
				mt::vec2f & pos = getLocalPositionModify();

				pos += way_offset;

				changePivot();
			}
		}

		if( m_rotate )
		{
			if( m_rotateTime < _timing  )
			{
				setLocalDirection( m_targetDir );

				m_rotate = false;

				this->callEvent("ROTATE_END", "()" );
			}
			else
			{
				m_rotateTime -= _timing;

				float t = _timing / float(m_rotateTime);

				const mt::vec2f & dir = getLocalDirection();

				mt::vec2f curr_dir = mt::slerp_v2_v2( dir, m_targetDir, t );

				setLocalDirection( curr_dir );

				changePivot();
			}
		}

		this->callEvent("UPDATE", "(k)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		this->registerEventMethod("MOVE_END", "onMoveEnd" );
		this->registerEventMethod("MOVE_STOP", "onMoveStop" );
		
		this->registerEventMethod("ROTATE_END", "onRotateEnd" );
		this->registerEventMethod("ROTATE_STOP", "onRotateStop" );

		this->registerEventMethod("UPDATE", "onUpdate" );

		this->callMethod("onActivate", "()" );

		return true;
	}
}
