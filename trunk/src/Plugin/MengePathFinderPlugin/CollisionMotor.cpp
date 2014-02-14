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
	inline static size_t s_rotateToState( float _angle )
	{
		float norm_angle = mt::angle_norm( _angle );

		for( size_t i = 0; i != 8; ++i )
		{
			float test_angle = mt::m_pi * 0.25f * i;

			float length_angle = mt::angle_length( norm_angle, test_angle );

			if( fabsf(length_angle) < 0.125f )
			{
				return i;
			}
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
		
		float angle = _object->getRotateX();
		size_t rotateState = s_rotateToState( angle );

		pybind::call( m_cb, "(OiOOO)"
			, pybind::ptr(_object)
			, rotateState
			, pybind::get_bool(m_rotating)
			, pybind::get_bool(m_transiting)
			, pybind::get_bool(m_finish)
			);
	}
}