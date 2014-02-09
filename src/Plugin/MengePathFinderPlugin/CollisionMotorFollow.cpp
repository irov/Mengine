#	include "CollisionMotorFollow.h"
#	include "CollisionObject.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionMotorFollow::CollisionMotorFollow()
		: m_target(nullptr)
		, m_minimalDistance(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorFollow::setTarget( CollisionObject * _target )
	{
		m_target = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionObject * CollisionMotorFollow::getTarget() const
	{
		return m_target;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorFollow::setMinimalDistance( float _distance )
	{
		m_minimalDistance = _distance;
	}
	//////////////////////////////////////////////////////////////////////////
	float CollisionMotorFollow::getMinimalDistance() const
	{
		return m_minimalDistance;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionMotorFollow::update( float _timing, CollisionObject * _object )
	{
		Node * obj_node = _object->getNode();
		mt::vec3f obj_pos = obj_node->getLocalPosition();

		Node * target_node = m_target->getNode();
		mt::vec3f target_pos = target_node->getLocalPosition();

		mt::vec3f diff_pos = target_pos - obj_pos;

		mt::vec2f dir;
		mt::norm_v2( dir, diff_pos.to_vec2f() );

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
		
		float length_pos = diff_pos.length();

		float obj_radius = _object->getRadius();
		float target_radius = m_target->getRadius();

		float needDistance = obj_radius + target_radius + m_minimalDistance;

		if( length_pos > needDistance )
		{
			float needStep = length_pos - needDistance;
		
			float step_pos = m_linearSpeed * _timing * 0.001f;

			float translate_pos = 0.f;
			if( step_pos >= needStep )
			{
				translate_pos = needStep;
			}
			else
			{
				translate_pos = step_pos;
			}

			mt::vec3f translate;
			translate.x = dir.x * translate_pos;
			translate.y = dir.y * translate_pos;
			translate.z = 0.f;

			obj_node->translate( translate );
		}
	}
}