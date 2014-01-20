#	include "Box2DPhysicSystem.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PhysicSystem, Menge::PhysicSystemInterface, Menge::Box2DPhysicSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicSystem::Box2DPhysicSystem()
        : m_serviceProvider(nullptr)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	Box2DPhysicSystem::~Box2DPhysicSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Box2DPhysicSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicSystem::initialize()
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::finalize()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	PhysicWorldInterface * Box2DPhysicSystem::createWorld( const ConstString & _name, float _gravityX, float _gravityY )
	{		
		for( TVectorWorlds::const_iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			const WorldDesc & desc = *it;

			if( desc.name == _name )
			{
				return nullptr;
			}
		}

		Box2DPhysicWorld * world = new Box2DPhysicWorld;
		
		world->initialize( _gravityX, _gravityY );

		WorldDesc desc;
		desc.name = _name;
		desc.world = world;

		m_worlds.push_back( desc );

		return world;
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicSystem::destroyWorld( const ConstString & _name )
	{
		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			const WorldDesc & desc = *it;

			if( desc.name != _name )
			{
				continue;
			}
			
			delete desc.world;

			m_worlds.erase( it );

			break;
		}		
	}
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::update( float _timing, int _velocityIterations, int _positionIterations )
    {
		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			const WorldDesc & desc = *it;

			desc.world->step( _timing, _velocityIterations , _positionIterations );
		}        

        //b2Contact* contact;
        //
        //for( TVectorContact::iterator 
        //    it = m_contacts.begin(),
        //    it_end = m_contacts.end(); 
        //it != it_end; 
        //++it )
        //{
        //    contact = *it;
        //    b2Body* body1 = contact->GetFixtureA()->GetBody();
        //    b2Body* body2 = contact->GetFixtureB()->GetBody();

        //    Box2DPhysicBody* mBody1 = static_cast<Box2DPhysicBody*>( body1->GetUserData() );
        //    Box2DPhysicBody* mBody2 = static_cast<Box2DPhysicBody*>( body2->GetUserData() );
        //    if( !body1 || !body2 )
        //    {
        //        continue;
        //    }
        //    mBody1->_collide( body2, contact );
        //    mBody2->_collide( body1, contact );
        //}
        //m_contacts.clear();
    }

}