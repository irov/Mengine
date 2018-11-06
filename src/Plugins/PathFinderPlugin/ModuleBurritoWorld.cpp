#include "ModuleBurritoWorld.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PlayerInterface.h"

#include "Kernel/ScriptWrapper.h"

#include "Kernel/NodePrototypeGenerator.h"

#include "pybind/pybind.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class FBurritoWorldDead
		{
		public:
			bool operator()( BurritoWorld * _world ) const
			{
				if( _world->isDead() == false )
				{
					return false;
				}

				_world->finalize();

				delete _world;

				return true;
			}

		private:
			FBurritoWorldDead & operator = (const FBurritoWorldDead & _name);
		};
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBurritoWorld::ModuleBurritoWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBurritoWorld::~ModuleBurritoWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBurritoWorld::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<BurritoBison>( kernel, "BurritoBison" )
			.def( "addForce", &BurritoBison::addForce )
			.def( "removeForce", &BurritoBison::removeForce )
			.def( "addImpulse", &BurritoBison::addImpulse )			
			.def( "getNode", &BurritoBison::getNode )
			.def( "getPosition", &BurritoBison::getPosition )
			.def( "setPositionY", &BurritoBison::setPositionY )
			.def( "getPositionY", &BurritoBison::getPositionY )
			.def( "getRadius", &BurritoBison::getRadius )
			.def( "setVelocity", &BurritoBison::setVelocity )
			.def( "getVelocity", &BurritoBison::getVelocity )
			.def( "setNeutron", &BurritoBison::setNeutron )
			.def( "getNeutron", &BurritoBison::getNeutron )
			.def( "setCollide", &BurritoBison::setCollide )
			.def( "getCollide", &BurritoBison::getCollide )
			.def( "addVelocityEvent", &BurritoBison::addVelocityEvent )
			.def( "removeVelocityEvent", &BurritoBison::removeVelocityEvent )
			.def( "removeAllVelocityEvents", &BurritoBison::removeAllVelocityEvents )
			.def( "addHeightEvent", &BurritoBison::addHeightEvent )
			.def( "removeHeightEvent", &BurritoBison::removeHeightEvent )
			.def( "removeAllHeightEvents", &BurritoBison::removeAllHeightEvents )
			.def( "addDistanceEvent", &BurritoBison::addDistanceEvent )
			.def( "removeDistanceEvent", &BurritoBison::removeDistanceEvent )
			.def( "removeAllDistanceEvents", &BurritoBison::removeAllDistanceEvents )
			.def( "setCameraSpeedMinimal", &BurritoBison::setCameraSpeedMinimal )
			.def( "getCameraSpeedMinimal", &BurritoBison::getCameraSpeedMinimal )
			.def( "setCameraSpeedMaximum", &BurritoBison::setCameraSpeedMaximum )
			.def( "getCameraSpeedMaximum", &BurritoBison::getCameraSpeedMaximum )
			.def( "setCameraScale", &BurritoBison::setCameraScale )
			.def( "getCameraScale", &BurritoBison::getCameraScale )
			.def( "setCameraOffset", &BurritoBison::setCameraOffset )
			.def( "getCameraOffset", &BurritoBison::getCameraOffset )
			.def( "setCameraFollowerScaleSpeed", &BurritoBison::setCameraFollowerScaleSpeed )
			.def( "getCameraFollowerScaleSpeed", &BurritoBison::getCameraFollowerScaleSpeed )
			.def( "getCameraFollowerScale", &BurritoBison::getCameraFollowerScale )
			;

		pybind::interface_<BurritoUnit>( kernel, "BurritoUnit" )
			.def( "getNode", &BurritoUnit::getNode )
			.def( "getPosition", &BurritoUnit::getPosition )
			.def( "setVelocity", &BurritoUnit::setVelocity )
			.def( "getVelocity", &BurritoUnit::getVelocity )
			.def( "setDead", &BurritoUnit::setDead )
			.def( "isDead", &BurritoUnit::isDead )
			.def( "setCollide", &BurritoUnit::setCollide )
			.def( "isCollide", &BurritoUnit::isCollide )
			.def( "setBound", &BurritoUnit::setBound )
			.def( "isBound", &BurritoUnit::isBound )
			;

		pybind::interface_<BurritoGround>( kernel, "BurritoGround" )
			;
			
		pybind::interface_<BurritoWorld>( kernel, "BurritoWorld" )
			.def( "setFreeze", &BurritoWorld::setFreeze )
			.def( "getFreeze", &BurritoWorld::getFreeze )
			.def( "createBison", &BurritoWorld::createBison )
			.def( "createGround", &BurritoWorld::createGround )
			.def( "addUnitBounds", &BurritoWorld::addUnitBounds )
			.def( "createLayer", &BurritoWorld::createLayer )
			.def( "addLayerUnit", &BurritoWorld::addLayerUnit )
			.def( "removeLayerUnit", &BurritoWorld::removeLayerUnit )
			;

		pybind::def_functor( kernel, "createBurritoWorld", this, &ModuleBurritoWorld::createBurritoWorld );
		pybind::def_functor( kernel, "removeBurritoWorld", this, &ModuleBurritoWorld::removeBurritoWorld );
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld * ModuleBurritoWorld::createBurritoWorld()
	{
		BurritoWorld * world = new BurritoWorld;

		if( world->initialize() == false )
		{
			return nullptr;
		}

		m_worldsAdd.emplace_back( world );

		return world;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::removeBurritoWorld( BurritoWorld * _world )
	{
		_world->setDead();
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::_tick( float _time, float _timing )
	{
		m_worlds.insert( m_worlds.end(), m_worldsAdd.begin(), m_worldsAdd.end() );
		m_worldsAdd.clear();

        for( BurritoWorld * world : m_worlds )
		{
			world->update( _time, _timing );
		}

		TVectorBurritoWorld::iterator it_erase = std::remove_if( m_worlds.begin(), m_worlds.end(), FBurritoWorldDead() );
		m_worlds.erase( it_erase, m_worlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::_render( const RenderContext * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//ToDo
	}
}