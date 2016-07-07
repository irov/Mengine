#	include "ModuleCollision.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ScriptClassWrapper.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleCollision::ModuleCollision()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleCollision::~ModuleCollision()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleCollision::_initialize()
	{
		pybind::interface_<CollisionWorld>( "CollisionWorld" )
			.def_smart_pointer()
			.def( "setIFFs", &CollisionWorld::setIFFs )
			.def( "getIFFs", &CollisionWorld::getIFFs )
			;

		pybind::def_functor( "createCollisionWorld", this, &ModuleCollision::createCollisionWorld );
		pybind::def_functor( "removeCollisionWorld", this, &ModuleCollision::removeCollisionWorld );

		pybind::interface_<NodeCollisionActor, pybind::bases<Node> >( "NodeCollisionActor", false )
			.def( "setCollisionRadius", &NodeCollisionActor::setCollisionRadius )
			.def( "getCollisionRadius", &NodeCollisionActor::getCollisionRadius )
			.def( "setCollisionRaycast", &NodeCollisionActor::setCollisionRaycast )
			.def( "getCollisionRaycast", &NodeCollisionActor::getCollisionRaycast )
			.def( "setCollisionIFF", &NodeCollisionActor::setCollisionIFF )
			.def( "getCollisionIFF", &NodeCollisionActor::getCollisionIFF )
			.def( "setCollisionActive", &NodeCollisionActor::setCollisionActive )
			.def( "getCollisionActive", &NodeCollisionActor::getCollisionActive )
			.def( "setCollisionWorld", &NodeCollisionActor::setCollisionWorld )
			.def( "getCollisionWorld", &NodeCollisionActor::getCollisionWorld )
			.def( "setCollisionUserData", &NodeCollisionActor::setCollisionUserData )
			.def( "getCollisionUserData", &NodeCollisionActor::getCollisionUserData )
			.def( "addCollisionException", &NodeCollisionActor::addCollisionException )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "NodeCollisionActor" ), new ClassScriptWrapper<NodeCollisionActor>() );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeCollisionActor" ), new NodePrototypeGenerator<NodeCollisionActor, 32>() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionWorldPtr ModuleCollision::createCollisionWorld()
	{
		CollisionWorldPtr collision = m_factoryCollisionWorld.createObject();

		if( collision->initialize() == false )
		{
			return nullptr;
		}

		m_collisionWorlds.push_back( collision );

		return collision;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::removeCollisionWorld( const CollisionWorldPtr & _collision )
	{
		_collision->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FMagnetDead
		{
			bool operator()( const CollisionWorldPtr & _collision ) const
			{
				if( _collision->isRemoved() == false )
				{
					return false;
				}

				return true;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;
		
		for( TVectorCollisionWorlds::iterator
			it = m_collisionWorlds.begin(),
			it_end = m_collisionWorlds.end();
		it != it_end;
		++it )
		{
			const CollisionWorldPtr & collision = *it;

			collision->update();
		}

		TVectorCollisionWorlds::iterator it_erase = std::remove_if( m_collisionWorlds.begin(), m_collisionWorlds.end(), FMagnetDead() );
		m_collisionWorlds.erase( it_erase, m_collisionWorlds.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleCollision::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_state;
		(void)_debugMask;
	}
}