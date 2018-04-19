#include "MagnetWorld.h"

#include "Factory/FactoryPool.h"

#include "math/ccd.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FActorDead
		{
			bool operator ()( const MagnetActorPtr & _actor ) const
			{
				return _actor->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	MagnetWorld::MagnetWorld()
		: m_remove(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MagnetWorld::~MagnetWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetWorld::initialize()
	{
		for( uint32_t i = 0; i != 8; ++i )
		{
			for( uint32_t j = 0; j != 8; ++j )
			{
				m_iffs[i][j] = false;
			}
		}

        m_factoryCollisionActor = new FactoryPool<MagnetActor, 32>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetWorld::setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value )
	{
		m_iffs[_iff1][_iff2] = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetWorld::getIFFs( uint32_t _iff1, uint32_t _iff2 ) const
	{
		return m_iffs[_iff1][_iff2];
	}
	//////////////////////////////////////////////////////////////////////////
	MagnetActorPtr MagnetWorld::createActor( MagnetActorProviderInterface * _provider, float _radius, float _force, uint32_t _iff, bool _active )
	{
		MagnetActorPtr actor = m_factoryCollisionActor->createObject();

		actor->setMagnetActorProvider( _provider );
		actor->setRadius( _radius );
		actor->setForce( _force );
		actor->setIFF( _iff );
		actor->setActive( _active );

		m_actorsAdd.emplace_back( actor );

		return actor;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetWorld::removeActor( const MagnetActorPtr & _actor )
	{
		_actor->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct collision_desc
		{
			MagnetActor * collision_actor;
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
	void MagnetWorld::update( float _time, float _timing )
	{
		(void)_time;

		m_actors.insert( m_actors.end(), m_actorsAdd.begin(), m_actorsAdd.end() );
		m_actorsAdd.clear();

		for( TVectorMagnetActor::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const MagnetActorPtr & actor = *it;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			if( actor->isActive() == false )
			{
				continue;
			}

			uint32_t actor_iff = actor->getIFF();
			
			float actor_force = actor->getForce();

			mt::vec3f actor_position;
			float actor_radius;
			actor->getSphere( actor_position, actor_radius );

			for( TVectorMagnetActor::iterator
				it_test = m_actors.begin(),
				it_test_end = m_actors.end();
			it_test != it_test_end;
			++it_test )
			{
				const MagnetActorPtr & actor_test = *it_test;

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
				actor_test->getSphere( actor_test_position, actor_test_radius );

				float length = mt::length_v3_v3( actor_position, actor_test_position );

				if( length > actor_radius )
				{
					continue;
				}

				mt::vec3f dir;
				mt::dir_v3_v3( dir, actor_test_position, actor_position );

				float speed = std::min( (actor_radius - length) / actor_radius * actor_force * _timing, length );
				
				mt::vec3f velocity = dir * speed;

				actor_test->addVelocity( velocity );				
			}
		}

		for( TVectorMagnetActor::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const MagnetActorPtr & actor = *it;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			actor->updateVelocity();
		}

		TVectorMagnetActor::iterator it_actors_erase = std::remove_if( m_actors.begin(), m_actors.end(), FActorDead() );
		m_actors.erase( it_actors_erase, m_actors.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetWorld::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetWorld::isRemoved() const
	{
		return m_remove;
	}
}
