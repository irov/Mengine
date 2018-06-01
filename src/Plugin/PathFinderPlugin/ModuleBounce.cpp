#include "ModuleBounce.h"

#include "Interface/ScriptSystemInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ScriptEventReceiver.h"
#include "Kernel/ScriptWrapper.h"

#include "Factory/FactoryPool.h"

#include "pybind/pybind.hpp"

#include <algorithm>

namespace Mengine
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

		pybind::interface_<BounceWorld, pybind::bases<Mixin> >( kernel, "BounceWorld" )
			.def( "createBounceActor", &BounceWorld::createBounceActor )
			.def( "removeBounceActor", &BounceWorld::removeBounceActor )
			;

		pybind::def_functor( kernel, "createBounceWorld", this, &ModuleBounce::createBounceWorld );
		pybind::def_functor( kernel, "removeBounceWorld", this, &ModuleBounce::removeBounceWorld );

		pybind::interface_<BounceActor, pybind::bases<Scriptable> >( kernel, "BounceActor", false )
			.def( "setRadius", &BounceActor::setRadius )
			.def( "getRadius", &BounceActor::getRadius )
			.def( "setMass", &BounceActor::setMass )
			.def( "getMass", &BounceActor::getMass )
			.def( "setNode", &BounceActor::setNode )
			.def( "getNode", &BounceActor::getNode )
			;

		SCRIPT_SERVICE()
			->setWrapper( STRINGIZE_STRING_LOCAL( "BounceActor" ), new ScriptWrapper<BounceActor>() );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "BounceActor" ), new DefaultPrototypeGenerator<BounceActor, 32>() );
		
        m_factoryBounceWorlds = new FactoryPool<BounceWorld, 4>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::_finalize()
	{
        SCRIPT_SERVICE()
            ->removeWrapper(STRINGIZE_STRING_LOCAL( "BounceActor"));

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "BounceActor" ) );

        m_factoryBounceWorlds = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	BounceWorldPtr ModuleBounce::createBounceWorld()
	{
		BounceWorldPtr world = m_factoryBounceWorlds->createObject();

		if( world->initialize() == false )
		{
			return nullptr;
		}

		m_bounceWorlds.emplace_back( world );

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

		for( const BounceWorldPtr & world : m_bounceWorlds )
		{
			world->update( _timing );
		}

		TVectorBounceWorlds::iterator it_erase = std::remove_if( m_bounceWorlds.begin(), m_bounceWorlds.end(), FBounceDead() );
		m_bounceWorlds.erase( it_erase, m_bounceWorlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBounce::_render( const RenderContext * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
	}
}