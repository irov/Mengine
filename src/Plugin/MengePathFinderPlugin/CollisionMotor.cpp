#	include "CollisionMotor.h"
#	include "CollisionObject.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionMotor::CollisionMotor()
		: m_linearSpeed(0.f)
		, m_angularSpeed(0.f)
		, m_moveStop(false)
		, m_cb(nullptr)
		, m_rotating(false)
		, m_transiting(false)
		, m_finish(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotor::~CollisionMotor()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotor::setCallback( PyObject * _cb )
	{
		pybind::decref( m_cb );
		m_cb = _cb;
		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotor::setLinearSpeed( float _speed )
	{
		m_linearSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	float CollisionMotor::getLinearSpeed() const
	{
		return m_linearSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotor::setAngularSpeed( float _speed )
	{
		m_angularSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	float CollisionMotor::getAngularSpeed() const
	{
		return m_angularSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotor::setMoveStop( bool _value )
	{
		m_moveStop = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionMotor::getMoveStop() const
	{
		return m_moveStop;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_rotateToState( float _angle )
	{
		float norm_angle = mt::angle_norm( _angle );

		float pi_deltha [] = {
			- mt::m_pi * 1.f / 12.f,
			mt::m_pi * 0.f / 12.f, 
			mt::m_pi * 1.f / 12.f,

			mt::m_pi * 1.f / 12.f,
			mt::m_pi * 2.f / 12.f, 
			mt::m_pi * 4.f / 12.f,
			
			mt::m_pi * 4.f / 12.f,
			mt::m_pi * 6.f / 12.f,
			mt::m_pi * 8.f / 12.f,

			mt::m_pi * 8.f / 12.f,
			mt::m_pi * 10.f / 12.f, 
			mt::m_pi * 11.f / 12.f,
			
			mt::m_pi * 11.f / 12.f,
			mt::m_pi * 12.f / 12.f,
			mt::m_pi * 13.f / 12.f,

			mt::m_pi * 13.f / 12.f,
			mt::m_pi * 14.f / 12.f,
			mt::m_pi * 16.f / 12.f,

			mt::m_pi * 16.f / 12.f,
			mt::m_pi * 18.f / 12.f,
			mt::m_pi * 20.f / 12.f,

			mt::m_pi * 20.f / 12.f,
			mt::m_pi * 22.f / 12.f,
			mt::m_pi * 23.f / 12.f,
		};


		for( uint32_t i = 0; i != 8; ++i )
		{
			float low_angle = pi_deltha[ i * 3 + 0];
			//float test_angle = pi_deltha[ i * 3 + 1];
			float hight_angle = pi_deltha[ i * 3 + 2];

			//float rad = mt::angle_norm360( norm_angle );

			if( mt::angle_length( norm_angle, low_angle ) > 0.f )
			{
				continue;
			}

			if( mt::angle_length( norm_angle, hight_angle ) < 0.f )
			{
				continue;
			}

			return i;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotor::updateState_( CollisionObject * _object, bool _rotating, bool _transiting, bool _finish )
	{
		if( m_cb == nullptr || pybind::is_none( m_cb ) == true )
		{
			return;
		}

		if( m_rotating == _rotating && m_transiting == _transiting && m_finish == _finish )
		{
			return;
		}

		m_rotating = _rotating;
		m_transiting = _transiting;
		m_finish = _finish;
		
		float angle = _object->getOrientationX();
		uint32_t rotateState = s_rotateToState( angle );

		pybind::call_t( m_cb, _object, rotateState, m_rotating, m_transiting, m_finish );
	}
}