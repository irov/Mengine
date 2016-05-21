#	pragma once

#	include "Core/ModuleBase.h"

#	include "CollisionWorld.h"
#	include "CollisionActor.h"
#	include "NodeCollisionActor.h"

namespace Menge
{
	class ModuleCollision
		: public ModuleBase
	{
	public:
		ModuleCollision();
		~ModuleCollision();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		CollisionWorldPtr createCollisionWorld();
		void removeCollisionWorld( const CollisionWorldPtr & _collision );

	public:
		void _update( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		typedef FactoryPoolStore<CollisionWorld, 4> TFactoryCollisionWorld;
		TFactoryCollisionWorld m_factoryCollisionWorld;

		typedef stdex::vector<CollisionWorldPtr> TVectorCollisionWorlds;
		TVectorCollisionWorlds m_collisionWorlds;
	};
}