#	include "Box2DModule.h"
#	include "Box2DWorld.h"
#	include "Box2DBody.h"

#	include "Kernel/ScriptClassWrapper.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	Box2DModule::Box2DModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DModule::~Box2DModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	bool Box2DModule::_initialize()
    {
		pybind::def_functor( "createBox2DWorld", this, &Box2DModule::createWorld );
		pybind::def_functor( "destroyBox2DWorld", this, &Box2DModule::destroyWorld );

		pybind::interface_<Box2DWorld>( "Box2DWorld" )
			.def( "setTimeStep", &Box2DWorld::setTimeStep )
			.def( "createBody", &Box2DWorld::createBody )
			.def( "destroyBody", &Box2DWorld::destroyBody )
			.def( "createDistanceJoint", &Box2DWorld::createDistanceJoint )
			.def( "createHingeJoint", &Box2DWorld::createHingeJoint )
			.def( "createPrismaticJoint", &Box2DWorld::createPrismaticJoint )
			.def( "createPulleyJoint", &Box2DWorld::createPulleyJoint )
			.def( "createGearJoint", &Box2DWorld::createGearJoint )
			.def( "createRopeJoint", &Box2DWorld::createRopeJoint )
			.def( "createWheelJoint", &Box2DWorld::createWheelJoint )
			.def( "destroyJoint", &Box2DWorld::destroyJoint )
			;
			
		pybind::interface_<Box2DBody>( "Box2DBody" )
			.def( "addShapeConvex", &Box2DBody::addShapeConvex )
			.def( "addShapeCircle", &Box2DBody::addShapeCircle )
			.def( "addShapeBox", &Box2DBody::addShapeBox )
			.def( "getMass", &Box2DBody::getMass )
			.def( "getInertia", &Box2DBody::getInertia )
			.def( "setLinearVelocity", &Box2DBody::setLinearVelocity )
			.def( "getLinearVelocity", &Box2DBody::getLinearVelocity )
			.def( "setAngularVelocity", &Box2DBody::setAngularVelocity )
			.def( "getAngularVelocity", &Box2DBody::getAngularVelocity )
			.def( "applyForce", &Box2DBody::applyForce )
			.def( "applyImpulse", &Box2DBody::applyImpulse )
			.def( "applyTorque", &Box2DBody::applyTorque )
			.def( "isFrozen", &Box2DBody::isFrozen )
			.def( "isSleeping", &Box2DBody::isSleeping )
			.def( "isStatic", &Box2DBody::isStatic )
			.def( "setLinearDumping", &Box2DBody::setLinearDumping )
			.def( "getLinearDumping", &Box2DBody::getLinearDumping )
			.def( "setAngularDumping", &Box2DBody::setAngularDumping )
			.def( "getAngularDumping", &Box2DBody::getAngularDumping )
			.def( "setFixedRotation", &Box2DBody::setFixedRotation )
			.def( "getFixedRotation", &Box2DBody::getFixedRotation )
			.def( "setIsBullet", &Box2DBody::setIsBullet )
			.def( "getIsBullet", &Box2DBody::getIsBullet )
			.def( "sleep", &Box2DBody::sleep )
			.def( "wakeUp", &Box2DBody::wakeUp )
			;

		pybind::interface_<Box2DJoint>( "Box2DJoint" )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ), new ClassScriptWrapper<Box2DBody>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DModule::_finalize()
    {
		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			Box2DWorld * world = *it;

			world->finalize();

			delete world;
		}

		m_worlds.clear();
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DWorld * Box2DModule::createWorld( const mt::vec2f& _gravity )
    {
		Box2DWorld * world = new Box2DWorld;

		if( world->initialize( _gravity ) == false )
		{
			return nullptr;
		}

		m_worlds.push_back( world );

		return world;
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DModule::destroyWorld( Box2DWorld * _world )
	{
		TVectorWorlds::iterator it_erase = std::find( m_worlds.begin(), m_worlds.end(), _world );
		m_worlds.erase( it_erase );

		_world->finalize();

		delete _world;
	}
    //////////////////////////////////////////////////////////////////////////
	void Box2DModule::_update( float _time, float _timing )
    {
		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			Box2DWorld * world = *it;

			world->update( _time, _timing );
		}
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DModule::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
}