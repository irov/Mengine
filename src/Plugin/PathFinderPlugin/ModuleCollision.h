#pragma once

#include "Core/ModuleBase.h"

#include "CollisionWorld.h"
#include "CollisionActor.h"
#include "NodeCollisionActor.h"

namespace Mengine
{
	class ModuleCollision
		: public ModuleBase
	{
	public:
		ModuleCollision();
		~ModuleCollision() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		CollisionWorldPtr createCollisionWorld();
		void removeCollisionWorld( const CollisionWorldPtr & _collision );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, uint32_t _debugMask ) override;
		
	protected:
		FactoryPtr m_factoryCollisionWorld;

		typedef stdex::vector<CollisionWorldPtr> TVectorCollisionWorlds;
		TVectorCollisionWorlds m_collisionWorlds;
	};
}