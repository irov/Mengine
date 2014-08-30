#	include "Box2DPhysicWorld.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Box2DPhysicWorld::Box2DPhysicWorld()
		: m_world(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Box2DPhysicWorld::initialize( float _gravityX, float _gravityY )
	{
		b2Vec2 gravity(_gravityX, _gravityY);
		m_world = new b2World( gravity );

		m_world->SetAllowSleeping( true );
		m_world->SetContactListener( this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::finalize()
	{
		delete m_world;
		m_world = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBodyInterface * Box2DPhysicWorld::createBody( EPBType _type )
	{
		b2BodyDef bodyDef;

		switch( _type )
		{
		case EPBT_STATIC:
			bodyDef.type = b2_staticBody;
			break;
		case EPBT_KINEMATIC:
			bodyDef.type = b2_kinematicBody;
			break;
		case EPBT_DYNAMIC:
			bodyDef.type = b2_dynamicBody;
			break;
		}						

		b2Body * b2_body = m_world->CreateBody( &bodyDef );			

		Box2DPhysicBody * body = new Box2DPhysicBody();
		body->initialize( b2_body );

		return body;
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::destroyBody( PhysicBodyInterface * _body )
	{
		delete static_cast<Box2DPhysicBody *>(_body);
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::step( float _timing, int _velocityIterations, int _positionIterations )
	{
		m_world->Step( _timing, _velocityIterations, _positionIterations );
	}
	//////////////////////////////////////////////////////////////////////////
	//This code may be wrong. because Box2D change collssion interface
	void Box2DPhysicWorld::BeginContact(b2Contact* _contact)
	{
		//TVectorContact::iterator it_find 
		//    = std::find_if( m_contacts.begin(), m_contacts.end(), FindContact( _contact ) );
		//if( it_find == m_contacts.end() )
		//{
		//    m_contacts.push_back( _contact );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::EndContact(b2Contact* _contact)
	{
		//TVectorContact::iterator it_find 
		//    = std::find_if( m_contacts.begin(), m_contacts.end(), FindContact( _contact ) );
		//if( it_find != m_contacts.end() )
		//{
		//    m_contacts.erase( it_find );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::PreSolve(b2Contact* _contact, const b2Manifold* _oldManifold)
	{
		(void)_contact;
		(void)_oldManifold;
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicWorld::PostSolve(b2Contact* _contact, const b2ContactImpulse* _impulse)
	{
		(void)_contact;
		(void)_impulse;
	}
}