#	include "Box2DModule.h"
#	include "Box2DWorld.h"
#	include "Box2DBody.h"
#	include "Box2DJoint.h"
#	include "NodeBox2DBody.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ScriptablePrototypeGenerator.h"

#	include "PythonScriptWrapper/ScriptClassWrapper.h"

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
			.def_smart_pointer()
			.def( "setTimeStep", &Box2DWorld::setTimeStep )
			.def( "createBody", &Box2DWorld::createBody )
			.def( "createDistanceJoint", &Box2DWorld::createDistanceJoint )
			.def( "createHingeJoint", &Box2DWorld::createHingeJoint )
			.def( "createPrismaticJoint", &Box2DWorld::createPrismaticJoint )
			.def( "createPulleyJoint", &Box2DWorld::createPulleyJoint )
			.def( "createGearJoint", &Box2DWorld::createGearJoint )
			.def( "createRopeJoint", &Box2DWorld::createRopeJoint )
			.def( "createWheelJoint", &Box2DWorld::createWheelJoint )
			.def( "rayCast", &Box2DWorld::rayCast )
			;
			
		pybind::interface_<Box2DBody, pybind::bases<Eventable> >( "Box2DBody" )
			.def_smart_pointer()
			.def( "setUserData", &Box2DBody::setUserData )
			.def( "getUserData", &Box2DBody::getUserData )
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
			.def_smart_pointer()
			;

		pybind::interface_<NodeBox2DBody, pybind::bases<Node> >( "NodeBox2DBody" )
			.def( "setBox2DBody", &NodeBox2DBody::setBox2DBody )
			.def( "getBox2DBody", &NodeBox2DBody::getBox2DBody )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeBox2DBody" ), new ClassScriptWrapper<NodeBox2DBody>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ), new ClassScriptWrapper<Box2DBody>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DJoint" ), new ClassScriptWrapper<Box2DJoint>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ), new ClassScriptWrapper<Box2DWorld>() );

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeBox2DBody" ), new NodePrototypeGenerator<NodeBox2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ), new ScriptablePrototypeGenerator<Box2DBody, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DJoint" ), new ScriptablePrototypeGenerator<Box2DJoint, 128> ) == false )
		{
			return false;
		}

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ), new ScriptablePrototypeGenerator<Box2DWorld, 128> ) == false )
		{
			return false;
		}
		
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
		Box2DWorld * world = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototypeT<Box2DWorld *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2D" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DWorld" ) );

		if( world->initialize( _gravity ) == false )
		{
			return nullptr;
		}

		m_worldsAdd.push_back( world );

		return world;
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DModule::destroyWorld( Box2DWorld * _world )
	{
		_world->setDead();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct FWorldDead
		{
			bool operator ()( Box2DWorld * _event ) const
			{
				return _event->isDead();
			}
		};
	}
    //////////////////////////////////////////////////////////////////////////
	void Box2DModule::_tick( float _time, float _timing )
    {
		m_worlds.insert( m_worlds.end(), m_worldsAdd.begin(), m_worldsAdd.end() );
		m_worldsAdd.clear();

		for( TVectorWorlds::iterator
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			Box2DWorld * world = *it;

			if( world->isDead() == true )
			{
				continue;
			}

			world->update( _time, _timing );
		}

		TVectorWorlds::iterator it_erase = std::remove_if( m_worlds.begin(), m_worlds.end(), FWorldDead() );
		m_worlds.erase( it_erase, m_worlds.end() );
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