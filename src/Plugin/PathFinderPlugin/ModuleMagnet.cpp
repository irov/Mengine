#include "ModuleMagnet.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ScriptEventReceiver.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ScriptWrapper.h"

#include "Factory/FactoryPool.h"

#include "pybind/pybind.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ModuleMagnet::ModuleMagnet()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleMagnet::~ModuleMagnet()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleMagnet::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<MagnetWorld>( kernel, "MagnetWorld" )
			.def_smart_pointer()
			.def( "setIFFs", &MagnetWorld::setIFFs )
			.def( "getIFFs", &MagnetWorld::getIFFs )
			;

		pybind::def_functor( kernel, "createMagnetWorld", this, &ModuleMagnet::createMagnetWorld );
		pybind::def_functor( kernel, "removeMagnetWorld", this, &ModuleMagnet::removeMagnetWorld );

		pybind::interface_<NodeMagnetActor, pybind::bases<Node> >( kernel, "NodeMagnetActor", false )
			.def( "setMagnetRadius", &NodeMagnetActor::setMagnetRadius )
			.def( "getMagnetRadius", &NodeMagnetActor::getMagnetRadius )
			.def( "setMagnetForce", &NodeMagnetActor::setMagnetForce )
			.def( "getMagnetForce", &NodeMagnetActor::getMagnetForce )
			.def( "setMagnetIFF", &NodeMagnetActor::setMagnetIFF )
			.def( "getMagnetIFF", &NodeMagnetActor::getMagnetIFF )
			.def( "setMagnetWorld", &NodeMagnetActor::setMagnetWorld )
			.def( "getMagnetWorld", &NodeMagnetActor::getMagnetWorld )
			.def( "setMagnetActive", &NodeMagnetActor::setMagnetActive )
			.def( "isMagnetActive", &NodeMagnetActor::isMagnetActive )
			.def( "setMagnetNodeAffect", &NodeMagnetActor::setMagnetNodeAffect )
			.def( "addMagnetException", &NodeMagnetActor::addMagnetException )
			;

		SCRIPT_SERVICE()
			->setWrapper( Helper::stringizeString( "NodeMagnetActor" ), new ScriptWrapper<NodeMagnetActor>() );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeMagnetActor" ), new NodePrototypeGenerator<NodeMagnetActor, 32> );

        m_factoryMagnetWorld = new FactoryPool<MagnetWorld, 4>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleMagnet::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MagnetWorldPtr ModuleMagnet::createMagnetWorld()
	{
		MagnetWorldPtr magnet = m_factoryMagnetWorld->createObject();

		if( magnet->initialize() == false )
		{
			return nullptr;
		}

		m_magnetWorlds.emplace_back( magnet );

		return magnet;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleMagnet::removeMagnetWorld( const MagnetWorldPtr & _magnet )
	{
		_magnet->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FMagnetDead
		{
			bool operator()( const MagnetWorldPtr & _magnet ) const
			{
				if( _magnet->isRemoved() == false )
				{
					return false;
				}

				return true;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleMagnet::_tick( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;
		
		for( TVectorMagnetWorlds::iterator
			it = m_magnetWorlds.begin(),
			it_end = m_magnetWorlds.end();
		it != it_end;
		++it )
		{
			const MagnetWorldPtr & magnet = *it;

			magnet->update( _time, _timing );
		}

		TVectorMagnetWorlds::iterator it_erase = std::remove_if( m_magnetWorlds.begin(), m_magnetWorlds.end(), FMagnetDead() );
		m_magnetWorlds.erase( it_erase, m_magnetWorlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleMagnet::_render( const RenderState * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
	}
}