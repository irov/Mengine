
#	include "PhysicEngine2D.h"
#	include "Interface/PhysicSystem2DInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::PhysicEngine2D( PhysicSystem2DInterface * _interface )
		: m_interface( _interface )
	{
		Holder<PhysicEngine2D>::keep( this );

		createScene( mt::vec2f( 0.0f, 0.0f ), mt::vec2f(2048.0f, 2048.0f), mt::vec2f(0.0f, 50.0f), true );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::~PhysicEngine2D()
	{
		destroyScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::createScene( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep /* = true  */)
	{
		m_interface->createWorld( &_upperLeft.x, &_lowerRight.x, &_gravity.x, _doSleep );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyScene()
	{
		m_interface->destroyWorld();
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface* PhysicEngine2D::createPhysicBody( const mt::polygon& _shape, const mt::vec2f& _pos, float _density, float _friction, float _restitution )
	{
		return m_interface->createPhysicBodyConvex( _shape.num_points(), &(_shape[0].x), _pos.x, _pos.y, 0.0f, _density, _friction, _restitution, 
												0.0f, 0.0f, 0.0f, 0xFFFF, 1, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyPhysicBody( PhysicBody2DInterface* _bodyInterface )
	{
		m_interface->destroyBody( _bodyInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::update( float _timing, int _iterations )
	{
		m_interface->update( _timing, _iterations );
	}
} // namespace Menge