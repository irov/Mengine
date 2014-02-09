#	include "CollisionMotorPosition.h"
#	include "CollisionObject.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorPosition::CollisionMotorPosition()
		: m_position(0.f, 0.f, 0.f)
		, m_angle(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorPosition::~CollisionMotorPosition()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::setPosition( const mt::vec3f & _position, float _angle )
	{
		m_position = _position;
		m_angle = _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorPosition::update( float _timing, CollisionObject * _object )
	{
		Node * obj_node = _object->getNode();
		mt::vec3f obj_pos = obj_node->getLocalPosition();

		mt::vec3f diff_pos = m_position - obj_pos;

		float length_pos = diff_pos.length();

		if( length_pos > 0.0001f )
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

			mt::vec2f dir;
			mt::norm_v2( dir, diff_pos.to_vec2f() );

			mt::vec3f translate;
			translate.x = dir.x * translate_pos;
			translate.y = dir.y * translate_pos;
			translate.z = 0.f;

			obj_node->translate( translate );


			float dir_angle = mt::signed_angle( dir );
			float obj_angle = obj_node->getRotateX();

			float diff_angle = mt::angle_length( obj_angle, dir_angle );

			float step_angle = m_angularSpeed * _timing * 0.001f;

			if( fabsf(step_angle - fabsf(diff_angle)) > 0.0001f )
			{
				if( step_angle >= fabsf(diff_angle) )
				{
					obj_angle = dir_angle;
				}
				else
				{
					obj_angle += step_angle * diff_angle;
				}

				obj_node->setRotateX( obj_angle );
			}
		}
		else
		{
			float obj_angle = obj_node->getRotateX();

			float diff_angle = mt::angle_length( obj_angle, m_angle );

			float step_angle = m_angularSpeed * _timing * 0.001f;

			if( fabsf(step_angle - fabsf(diff_angle)) > 0.0001f )
			{
				if( step_angle >= fabsf(diff_angle) )
				{
					obj_angle = m_angle;
				}
				else
				{
					obj_angle += step_angle * diff_angle;
				}

				obj_node->setRotateX( obj_angle );
			}
		}
	}
}