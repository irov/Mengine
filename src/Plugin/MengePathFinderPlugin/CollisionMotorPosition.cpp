#	include "CollisionMotorPosition.h"
#	include "CollisionObject.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorPosition::CollisionMotorPosition()
		: m_position(0.f, 0.f, 0.f)
		, m_angle(0.f)
		, m_cb(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorPosition::~CollisionMotorPosition()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::setCallback( PyObject * _cb )
	{
		pybind::decref( m_cb );
		m_cb = _cb;
		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::setPositionAngle( const mt::vec3f & _position, float _angle )
	{
		m_position = _position;
		m_angle = _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::setPositionLookAt( const mt::vec3f & _position, const mt::vec3f & _lookAt )
	{
		mt::mat4f mlookat;
		mt::make_lookat_m4( mlookat, _position, _lookAt - _position, mt::vec3f(0.f, 1.f, 0.f), 1.f );

		mt::vec3f euler;
		mt::make_euler_angles( euler, mlookat );

		this->setPositionAngle( _position, euler.x );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::update( float _timing, CollisionObject * _object )
	{
		bool rotating = false;
		bool translating = false;
		bool finish = false;

		mt::vec3f obj_pos = _object->getWorldPosition();

		mt::vec3f diff_pos = m_position - obj_pos;

		float length_pos = diff_pos.length();

		if( length_pos > 0.0001f )
		{
			mt::vec2f dir;
			mt::norm_v2_v2( dir, diff_pos.to_vec2f() );

			if( m_moveStop == false )
			{
				float step_pos = m_linearSpeed * _timing * 0.001f;

				float translate_pos = 0.f;
				if( step_pos >= length_pos )
				{
					translate_pos = length_pos;
				}
				else
				{
					translate_pos = step_pos;
				}

				mt::vec3f translate;
				translate.x = dir.x * translate_pos;
				translate.y = dir.y * translate_pos;
				translate.z = 0.f;

				_object->translate( translate );
				translating = true;
			}
			
			float dir_angle = mt::signed_angle( dir );
			float obj_angle = _object->getRotateX();

			float diff_angle = mt::angle_length( obj_angle, dir_angle );

			float step_angle = m_angularSpeed * _timing * 0.001f;

			if( fabsf(diff_angle) > 0.0001f )
			{
				if( step_angle >= fabsf(diff_angle) )
				{
					obj_angle = dir_angle;
				}
				else
				{
					obj_angle += step_angle * diff_angle;
				}

				_object->setRotateX( obj_angle );
				rotating = true;
			}
		}
		else
		{
			float obj_angle = _object->getRotateX();

			float diff_angle = mt::angle_length( obj_angle, m_angle );

			float step_angle = m_angularSpeed * _timing * 0.001f;
						
			if( fabsf(diff_angle) > 0.0001f )
			{
				if( step_angle >= fabsf(diff_angle) )
				{
					obj_angle = m_angle;
					finish = true;
				}
				else
				{
					obj_angle += step_angle * diff_angle;					
				}

				_object->setRotateX( obj_angle );
				rotating = true;
			}
			else
			{
				finish = true;
			}
		}

		this->updateState_( _object, rotating, translating, finish );
	}
}