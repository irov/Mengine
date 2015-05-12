#	include "ModuleBurritoWorld.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "Kernel/ScriptClassWrapper.h"
#	include "Kernel/NodePrototypeGenerator.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
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

				return true;
			}

		private:
			FBurritoWorldDead & operator = (const FBurritoWorldDead & _name);
		};
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBurritoWorld::ModuleBurritoWorld()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBurritoWorld::~ModuleBurritoWorld()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ModuleBurritoWorld::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBurritoWorld::initialize()
	{
		pybind::interface_<BurritoBison>("BurritoBison")
			.def( "addForce", &BurritoBison::addForce )
			.def( "removeForce", &BurritoBison::removeForce )
			.def( "addImpulse", &BurritoBison::addImpulse )			
			.def( "getNode", &BurritoBison::getNode )
			.def( "getPosition", &BurritoBison::getPosition )
			.def( "getOffset", &BurritoBison::getOffset )
			.def( "getRadius", &BurritoBison::getRadius )
			.def( "setVelocity", &BurritoBison::setVelocity )
			.def( "getVelocity", &BurritoBison::getVelocity )
			.def( "setNeutron", &BurritoBison::setNeutron )
			.def( "getNeutron", &BurritoBison::getNeutron )
			.def( "addVelocityEvent", &BurritoBison::addVelocityEvent )
			.def( "removeAllVelocityEvents", &BurritoBison::removeAllVelocityEvents )
			;

		pybind::interface_<BurritoUnit>( "BurritoUnit" )
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
			
		pybind::interface_<BurritoWorld>("BurritoWorld")
			.def( "setFreeze", &BurritoWorld::setFreeze )
			.def( "getFreeze", &BurritoWorld::getFreeze )
			.def( "createBison", &BurritoWorld::createBison )
			.def( "createGround", &BurritoWorld::createGround )
			.def( "addUnitBounds", &BurritoWorld::addUnitBounds )
			.def( "createLayer", &BurritoWorld::createLayer )
			.def( "addLayerNode", &BurritoWorld::addLayerNode )
			.def( "removeLayerNode", &BurritoWorld::removeLayerNode )
			.def( "addLayerUnit", &BurritoWorld::addLayerUnit )
			.def( "removeLayerUnit", &BurritoWorld::removeLayerUnit )
			;

		pybind::def_functor( "createBurritoWorld", this, &ModuleBurritoWorld::createBurritoWorld );
		pybind::def_functor( "removeBurritoWorld", this, &ModuleBurritoWorld::removeBurritoWorld );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModuleBurritoWorld::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld * ModuleBurritoWorld::createBurritoWorld()
	{
		BurritoWorld * world = new BurritoWorld;

		m_worldsAdd.push_back( world );

		return world;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::removeBurritoWorld( BurritoWorld * _world )
	{
		_world->setDead();
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::update( float _time, float _timing )
	{
		m_worlds.insert( m_worlds.end(), m_worldsAdd.begin(), m_worldsAdd.end() );
		m_worldsAdd.clear();

		for( TVectorBurritoWorld::iterator 
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			BurritoWorld * world = *it;

			world->update( _time, _timing );
		}

		TVectorBurritoWorld::iterator it_erase = std::remove_if( m_worlds.begin(), m_worlds.end(), FBurritoWorldDead() );
		m_worlds.erase( it_erase, m_worlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		(void)_viewport;
		(void)_camera;
		(void)_debugMask;
		//ToDo
	}
}