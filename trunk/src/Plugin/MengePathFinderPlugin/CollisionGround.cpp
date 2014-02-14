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
	void CollisionGround::addObject( CollisionObject * _object )
	{
		m_objectsAdd.push_back( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeObject( CollisionObject * _object )
	{
		this->removeObject_( _object );
		this->removeObjectAdd_( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeObject_( CollisionObject * _object )
	{
		TVectorCollisionObjects::iterator it_found = std::find( m_objects.begin(), m_objects.end(), _object );

		if( it_found == m_objects.end() )
		{
			return;
		}

		*it_found = m_objects.back();
		m_objects.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeObjectAdd_( CollisionObject * _object )
	{
		TVectorCollisionObjects::iterator it_found = std::find( m_objectsAdd.begin(), m_objectsAdd.end(), _object );

		if( it_found == m_objectsAdd.end() )
		{
			return;
		}

		*it_found = m_objectsAdd.back();
		m_objectsAdd.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::addRadar( CollisionRadar * _radar )
	{
		m_radarsAdd.push_back( _radar );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeRadar( CollisionRadar * _radar )
	{
		this->removeRadar_( _radar );
		this->removeRadarAdd_( _radar );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::removeRadar_( CollisionRadar * _radar )
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
	void CollisionGround::removeRadarAdd_( CollisionRadar * _radar )
	{
		TVectorCollisionRadars::iterator it_found = std::find( m_radarsAdd.begin(), m_radarsAdd.end(), _radar );

		if( it_found == m_radarsAdd.end() )
		{
			return;
		}

		*it_found = m_radarsAdd.back();
		m_radarsAdd.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::update( float _timing )
	{
		m_objects.insert( m_objects.end(), m_objectsAdd.begin(), m_objectsAdd.end() );
		m_objectsAdd.clear();

		m_radars.insert( m_radars.end(), m_radarsAdd.begin(), m_radarsAdd.end() );
		m_radarsAdd.clear();

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * object = *it;

			if( object->getDead() == true )
			{
				continue;
			}
			
			object->update( _timing );						
		}

		for( TVectorCollisionObjects::size_type
			it = 0,
			it_end = m_objects.size();
		it != it_end;
		++it )
		{
			CollisionObject * i_object = m_objects[it];

			if( i_object->getDead() == true )
			{
				continue;
			}

			if( i_object->getGhost() == true )
			{
				continue;
			}

			for( TVectorCollisionObjects::size_type
				jt = it + 1,
				jt_end = m_objects.size();
			jt != jt_end;
			++jt )
			{
				CollisionObject * j_object = m_objects[jt];

				if( j_object->getDead() == true )
				{
					continue;
				}

				if( j_object->getGhost() == true )
				{
					continue;
				}
				
				this->collisionTest_( i_object, j_object, _timing );
			}
		}		

		for( TVectorCollisionRadars::iterator
			it = m_radars.begin(),
			it_end = m_radars.end();
		it != it_end;
		++it )
		{
			CollisionRadar * radar = *it;

			if( radar->getDead() == true )
			{
				continue;
			}

			this->collisionRadar_( radar );
		}

		class FEraseDeadObject
		{
		public:
			bool operator()( CollisionObject * _object ) 
			{ 
				return _object->getDead();
			}
		};

		m_objects.erase( std::remove_if( m_objects.begin(), m_objects.end(), FEraseDeadObject()), m_objects.end() );

		class FEraseDeadRadar
		{
		public:
			bool operator()( CollisionRadar * _radar ) 
			{ 
				return _radar->getDead();
			}
		};

		m_radars.erase( std::remove_if( m_radars.begin(), m_radars.end(), FEraseDeadRadar()), m_radars.end() );
	}	
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::collisionTest_( CollisionObject * _object1, CollisionObject * _object2, float _timing )
	{
		mt::vec3f test_position = _object1->getWorldPosition();

		float test_radius = _object1->getRadius();
		float test_mass = _object1->getMass();

		mt::vec3f position = _object2->getWorldPosition();

		float radius = _object2->getRadius();

		float test_distance = mt::length_v3_v3( test_position, position );
		float correct_distance = radius + test_radius;
		if( test_distance >= correct_distance )
		{
			return;
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

		float mass = _object2->getMass();
		if( test_mass == 0.f && mass == 0.f )
		{
			test_position -= test_dir * diff_distance * 0.5f * speed_undiff;
			position += test_dir * diff_distance * 0.5f * speed_undiff;

			_object1->setWorldPosition( test_position );
			_object2->setWorldPosition( position );
		}
		else if( test_mass == 0.f )
		{
			position += test_dir * diff_distance * 1.f * speed_undiff;

			_object2->setWorldPosition( position );
		}
		else if( mass == 0.f )
		{
			test_position -= test_dir * diff_distance * 1.f * speed_undiff;

			_object2->setWorldPosition( test_position );
		}
		else
		{
			float sum_mass = test_mass + mass;
			test_position -= test_dir * diff_distance * mass / sum_mass * speed_undiff;
			position += test_dir * diff_distance * test_mass / sum_mass * speed_undiff;

			_object1->setWorldPosition( test_position );
			_object2->setWorldPosition( position );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionGround::collisionRadar_( CollisionRadar * _radar )
	{
		float radar_radius = _radar->getRadius();

		const mt::vec3f & radar_position = _radar->getWorldPosition();

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObject * object = *it;
			
			if( object->getDead() == true )
			{				
				_radar->removeObject( object );
				continue;
			}

			if( object->getGhost() == true )
			{
				continue;
			}

			const mt::vec3f & position = object->getWorldPosition();
			
			float test_distance = mt::length_v3_v3( radar_position, position );

			float radius = object->getRadius();
			float correct_distance = radar_radius + radius;

			if( test_distance > correct_distance )
			{
				_radar->removeObject( object );
			}
			else
			{
				_radar->insertObject( object );
			}			
		}
	}
}