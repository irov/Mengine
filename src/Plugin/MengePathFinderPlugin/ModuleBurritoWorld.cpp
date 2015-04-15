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
			.def( "getVelocity", &BurritoBison::getVelocity )
			;

		pybind::interface_<BurritoWorld>("BurritoWorld")
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

		m_worlds.push_back( world );

		return world;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::removeBurritoWorld( BurritoWorld * _world )
	{
		TVectorBurritoWorld::iterator it_erase = std::find( m_worlds.begin(), m_worlds.end(), _world );

		if( it_erase == m_worlds.end() )
		{
			return;
		}

		*it_erase = m_worlds.back();
		m_worlds.pop_back();

		delete _world;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBurritoWorld::update( float _time, float _timing )
	{
		for( TVectorBurritoWorld::iterator 
			it = m_worlds.begin(),
			it_end = m_worlds.end();
		it != it_end;
		++it )
		{
			BurritoWorld * world = *it;

			world->update( _time, _timing );
		}
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