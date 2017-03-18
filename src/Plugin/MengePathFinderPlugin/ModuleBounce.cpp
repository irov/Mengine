#	include "ModuleBounce.h"

#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Kernel/DefaultPrototypeGenerator.h"
#	include "Kernel/ScriptEventReceiver.h"
#   include "Kernel/ScriptWrapper.h"

#   include "Factory/FactoryPool.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleBounce::ModuleBounce()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBounce::~ModuleBounce()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBounce::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<BounceWorld>( kernel, "BounceWorld" )
			.def_smart_pointer()
			.def( "createBounceActor", &BounceWorld::createBounceActor )
			.def( "removeBounceActor", &BounceWorld::removeBounceActor )
			;

		pybind::def_functor( kernel, "createBounceWorld", this, &ModuleBounce::createBounceWorld );
		pybind::def_functor( kernel, "removeBounceWorld", this, &ModuleBounce::removeBounceWorld );

		pybind::interface_<BounceActor>( kernel, "BounceActor", false )
			.def_smart_pointer()
			.def( "setRadius", &BounceActor::setRadius )
			.def( "getRadius", &BounceActor::getRadius )
			.def( "setMass", &BounceActor::setMass )
			.def( "getMass", &BounceActor::getMass )
			.def( "setNode", &BounceActor::setNode )
			.def( "getNode", &BounceActor::getNode )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "BounceActor" ), new ScriptWrapper<BounceActor>() );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "BounceActor" ), new DefaultPrototypeGenerator<BounceActor, 32>() );
		
        m_factoryBounceWorlds = new FactoryPool<BounceWorld, 4>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BounceWorldPtr ModuleBounce::createBounceWorld()
	{
		BounceWorldPtr world = m_factoryBounceWorlds->createObject();

		if( world->initialize( m_serviceProvider ) == false )
		{
			return nullptr;
		}

		m_bounceWorlds.push_back( world );

		return world;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::removeBounceWorld( const BounceWorldPtr & _world )
	{
		_world->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FBounceDead
		{
			bool operator()( const BounceWorldPtr & _world ) const
			{
				return _world->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::_tick( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		for( TVectorBounceWorlds::iterator
			it = m_bounceWorlds.begin(),
			it_end = m_bounceWorlds.end();
		it != it_end;
		++it )
		{
			const BounceWorldPtr & world = *it;

			world->update( _timing );
		}

		TVectorBounceWorlds::iterator it_erase = std::remove_if( m_bounceWorlds.begin(), m_bounceWorlds.end(), FBounceDead() );
		m_bounceWorlds.erase( it_erase, m_bounceWorlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_state;
		(void)_debugMask;
	}
}