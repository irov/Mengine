#	include "CollisionWorld.h"

#	include "Factory/FactoryPool.h"

#	include "Math/ccd.h"

#   include <algorithm>

#	ifndef MENGINE_COLLISION_WORLD_PENETRATION_COUNT
#	define MENGINE_COLLISION_WORLD_PENETRATION_COUNT 64
#	endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FActorDead
		{
			bool operator ()( const CollisionActorPtr & _actor ) const
			{
				return _actor->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorld::CollisionWorld()
		: m_remove(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorld::~CollisionWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionWorld::initialize()
	{
		for( uint32_t i = 0; i != 8; ++i )
		{
			for( uint32_t j = 0; j != 8; ++j )
			{
				m_iffs[i][j] = false;
			}
		}

        m_factoryCollisionActor = new FactoryPool<CollisionActor, 32>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value )
	{
		m_iffs[_iff1][_iff2] = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionWorld::getIFFs( uint32_t _iff1, uint32_t _iff2 ) const
	{
		return m_iffs[_iff1][_iff2];
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionActorPtr CollisionWorld::createActor( CollisionActorProviderInterface * _provider, float _radius, const mt::vec3f & _raycastDirection, uint32_t _iff, bool _active )
	{
		CollisionActorPtr actor = m_factoryCollisionActor->createObject();

		actor->setCollisionActorProvider( _provider );
		actor->setRadius( _radius );
		actor->setRaycastDirection( _raycastDirection );
		actor->setIFF( _iff );
		actor->setActive( _active );
		
		actor->initialize();

		m_actorsAdd.push_back( actor );

		return actor;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::removeActor( const CollisionActorPtr & _actor )
	{
		_actor->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct collision_desc
		{
			CollisionActor * collision_actor;
			mt::vec3f collision_point;
			mt::vec3f collision_normal;
			float collision_time;
			float hit_time;
		};

		struct collision_pred
		{
			bool operator()( const collision_desc & a, const collision_desc & b ) const
			{
				return a.collision_time < b.collision_time;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::update()
	{
		m_actors.insert( m_actors.end(), m_actorsAdd.begin(), m_actorsAdd.end() );
		m_actorsAdd.clear();

		for( TVectorCollisionActor::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const CollisionActorPtr & actor = *it;
			
			if( actor->isRemoved() == true )
			{
				continue;
			}

			actor->update();
		}

		for( TVectorCollisionActor::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const CollisionActorPtr & actor = *it;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			if( actor->isActive() == false )
			{
				continue;
			}

			uint32_t actor_iff = actor->getIFF();

			mt::vec3f actor_position;
			float actor_radius;
			mt::vec3f actor_velocity;
			actor->getSphereCCD( actor_position, actor_radius, actor_velocity );

			CollisionActorProviderInterface * actor_provider = actor->getCollisionActorProvider();

			uint32_t collision_count = 0;
			collision_desc collisions[MENGINE_COLLISION_WORLD_PENETRATION_COUNT];
			
			for( TVectorCollisionActor::iterator
				it_test = m_actors.begin(),
				it_test_end = m_actors.end();
			it_test != it_test_end;
			++it_test )
			{
				const CollisionActorPtr & actor_test = *it_test;

				if( actor_test->isRemoved() == true )
				{
					continue;
				}

				if( actor_test == actor )
				{
					continue;
				}

				if( actor->isException( actor_test ) == true )
				{
					continue;
				}

				uint32_t actor_test_iff = actor_test->getIFF();

				if( m_iffs[actor_iff][actor_test_iff] == false )
				{
					continue;
				}

				mt::vec3f actor_test_position;
				float actor_test_radius;
				mt::vec3f actor_test_velocity;
				actor_test->getSphereCCD( actor_test_position, actor_test_radius, actor_test_velocity );

				float test_collision_time;
				mt::vec3f test_collision_normal;

				if( mt::ccd_sphere_sphere( actor_position, actor_radius, actor_velocity, actor_test_position, actor_test_radius, actor_test_velocity, test_collision_time, test_collision_normal ) == false )
				{
					continue;
				}

				if( test_collision_time > 1.f )
				{
					continue;
				}

				mt::vec3f test_collision_point = actor_position + actor_velocity * test_collision_time;

				float length_actor_velocity = mt::length_v3( actor_velocity );

				float hit_time = test_collision_time + actor_radius / length_actor_velocity;

				collision_desc & desc = collisions[collision_count++];

				desc.collision_actor = actor_test.get();
				desc.collision_point = test_collision_point;
				desc.collision_normal = test_collision_normal;
				desc.collision_time = test_collision_time;
				desc.hit_time = hit_time;

				if( collision_count == MENGINE_COLLISION_WORLD_PENETRATION_COUNT )
				{
					break;
				}
			}

			std::sort( collisions, collisions + collision_count, collision_pred() );

			for( uint32_t i = 0; i != collision_count; ++i )
			{
				const collision_desc & desc = collisions[i];

				if( actor->isRemoved() == true )
				{
					break;
				}

				CollisionActor * collision_actor = desc.collision_actor;

				if( collision_actor->isRemoved() == true )
				{
					continue;
				}

				CollisionActorProviderInterface * actor_collision_provider = collision_actor->getCollisionActorProvider();

				if( actor_provider->onCollisionTest( actor_collision_provider, desc.collision_point, desc.collision_normal, desc.hit_time ) == true )
				{
					break;
				}
			}
		}

		TVectorCollisionActor::iterator it_actors_erase = std::remove_if( m_actors.begin(), m_actors.end(), FActorDead() );
		m_actors.erase( it_actors_erase, m_actors.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionWorld::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionWorld::isRemoved() const
	{
		return m_remove;
	}
}