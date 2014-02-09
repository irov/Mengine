#	include "CollisionGround.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionGround::CollisionGround()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionGround::~CollisionGround()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::addObject( CollisionObject * _motor )
	{
		m_objects.push_back( _motor );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeObject( CollisionObject * _motor )
	{
		TVectorCollisionObjects::iterator it_found = std::find( m_objects.begin(), m_objects.end(), _motor );

		if( it_found == m_objects.end() )
		{
			return;
		}

		*it_found = m_objects.back();
		m_objects.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::addRadar( CollisionRadar * _radar )
	{
		m_radars.push_back( _radar );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeRadar( CollisionRadar * _radar )
	{
		TVectorCollisionRadars::iterator it_found = std::find( m_radars.begin(), m_radars.end(), _radar );

		if( it_found == m_radars.end() )
		{
			return;
		}

		*it_found = m_radars.back();
		m_radars.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::update( float _timing )
	{
		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * motor = *it;

			motor->update( _timing );						
		}

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * motor = *it;

			this->collisionTest_( motor, _timing );
		}		

		for( TVectorCollisionRadars::iterator
			it = m_radars.begin(),
			it_end = m_radars.end();
		it != it_end;
		++it )
		{
			CollisionRadar * radar = *it;

			this->collisionRadar_( radar );
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::collisionTest_( CollisionObject * _object, float _timing )
	{
		Node * test_node = _object->getNode();
		mt::vec3f test_position = test_node->getLocalPosition();

		float test_radius = _object->getRadius();
		float test_mass = _object->getMass();

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * object = *it;

			if( object == _object )
			{
				continue;
			}
			
			Node * node = object->getNode();
			mt::vec3f position = node->getLocalPosition();
			
			float radius = object->getRadius();

			float test_distance = mt::length_v3_v3( test_position, position );
			float correct_distance = radius + test_radius;
			if( test_distance >= correct_distance )
			{
				continue;
			}

			if( test_distance < 0.001f )
			{
				test_position.x += 0.1f;

				test_distance = mt::length_v3_v3( test_position, position );
			}
			
			float diff_distance = correct_distance - test_distance;

			mt::vec3f test_dir;
			mt::norm_v3( test_dir, position - test_position );

			float speed_undiff = 10.f * _timing * 0.001f;

			float mass = object->getMass();
			if( test_mass == 0.f && mass == 0.f )
			{
				test_position -= test_dir * diff_distance * 0.5f * speed_undiff;
				position += test_dir * diff_distance * 0.5f * speed_undiff;

				test_node->setLocalPosition( test_position );
				node->setLocalPosition( position );
			}
			else if( test_mass == 0.f )
			{
				position += test_dir * diff_distance * 1.f * speed_undiff;

				node->setLocalPosition( position );
			}
			else if( mass == 0.f )
			{
				test_position -= test_dir * diff_distance * 1.f * speed_undiff;

				test_node->setLocalPosition( test_position );
			}
			else
			{
				float sum_mass = test_mass + mass;
				test_position -= test_dir * diff_distance * mass / sum_mass * speed_undiff;
				position += test_dir * diff_distance * test_mass / sum_mass * speed_undiff;

				test_node->setLocalPosition( test_position );
				node->setLocalPosition( position );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::collisionRadar_( CollisionRadar * _radar )
	{
		Node * radar_node = _radar->getNode();
		float radar_radius = _radar->getRadius();

		const mt::vec3f & radar_position = radar_node->getLocalPosition();

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * object = *it;

			Node * node = object->getNode();
			mt::vec3f position = node->getLocalPosition();

			float radius = object->getRadius();

			float test_distance = mt::length_v3_v3( radar_position, position );
			float correct_distance = radius + radar_radius;
			if( test_distance >= correct_distance )
			{
				continue;
			}

			_radar->onRadarObjectEnter( object );
		}
	}
}