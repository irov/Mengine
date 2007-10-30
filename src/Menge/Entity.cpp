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

		this->callEvent("MOVE_END", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_update( size_t _timing, const Viewport & _viewport )
	{
		if( m_moveTo )
		{
			if( m_moveTime < _timing )
			{
				setLocalPosition( m_movePoint );

				moveStop();
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

		this->callEvent("UPDATE", "(k)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		this->registerEventMethod("MOVE_END", "onMoveEnd" );
		this->registerEventMethod("UPDATE", "onUpdate" );

		this->callMethod("onActivate", "()" );

		return true;
	}
}
