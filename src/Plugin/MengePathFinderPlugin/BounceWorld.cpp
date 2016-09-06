#	include "BounceWorld.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FActorDead
		{
			bool operator ()( const BounceActorPtr & _actor ) const
			{
				return _actor->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	BounceWorld::BounceWorld()
		: m_serviceProvider( nullptr )
		, m_scriptWrapper( nullptr )
		, m_remove( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BounceWorld::~BounceWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BounceWorld::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_scriptWrapper = SCRIPT_SERVICE( m_serviceProvider )
			->getWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "BounceActor" ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BounceWorld::finalize()
	{
		m_actors.clear();
		m_actorsAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	BounceActorPtr BounceWorld::createBounceActor( float _radius, float _mass, Node * _node )
	{
		BounceActorPtr actor = m_factoryBounceActors.createObject();

		actor->setScriptWrapper( m_scriptWrapper );

		actor->setRadius( _radius );
		actor->setMass( _mass );
		actor->setNode( _node );

		m_actorsAdd.push_back( actor );

		return actor;
	}
	//////////////////////////////////////////////////////////////////////////
	void BounceWorld::removeBounceActor( const BounceActorPtr & _actor )
	{
		_actor->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	void BounceWorld::update( float _timing )
	{
		m_actors.insert( m_actors.end(), m_actorsAdd.begin(), m_actorsAdd.end() );
		m_actorsAdd.clear();

		for( TVectorBounceActors::iterator
			it = m_actors.begin(),
			it_end = m_actors.end();
		it != it_end;
		++it )
		{
			const BounceActorPtr & actor = *it;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			float actor_radius = actor->getRadius();
			float actor_mass = actor->getMass();

			Node * actor_node = actor->getNode();

			const mt::vec3f & actor_position = actor_node->getWorldPosition();

			for( TVectorBounceActors::iterator
				it_test = m_actors.begin(),
				it_test_end = m_actors.end();
			it_test != it_test_end;
			++it_test )
			{
				const BounceActorPtr & test = *it_test;

				if( test->isRemoved() == true )
				{
					continue;
				}

				if( test == actor )
				{
					continue;
				}
				
				float test_radius = test->getRadius();
				float test_mass = test->getMass();

				Node * test_node = test->getNode();

				const mt::vec3f & test_position = test_node->getWorldPosition();

				float sqrlength = mt::sqrlength_v3_v3( actor_position, test_position );

				if( sqrlength > (actor_radius + test_radius) * (actor_radius + test_radius) )
				{
					continue;
				}

				float length = ::sqrtf( sqrlength );

				mt::vec3f direction;
				mt::dir_v3_v3( direction, actor_position, test_position );

				float diff_length = (actor_radius + test_radius - length);

				float actor_bounce = - actor_mass / (actor_mass + test_mass) * diff_length * _timing * 0.01f;
				actor_node->translate( direction * actor_bounce );

				float test_bounce = test_mass / (actor_mass + test_mass) * diff_length * _timing * 0.01f;
				test_node->translate( direction * test_bounce );
			}
		}

		TVectorBounceActors::iterator it_actors_erase = std::remove_if( m_actors.begin(), m_actors.end(), FActorDead() );
		m_actors.erase( it_actors_erase, m_actors.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void BounceWorld::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BounceWorld::isRemoved() const
	{
		return m_remove;
	}
}