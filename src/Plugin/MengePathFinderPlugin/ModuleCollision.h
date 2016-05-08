#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "CollisionWorld.h"
#	include "CollisionActor.h"
#	include "NodeCollisionActor.h"

namespace Menge
{
	class ModuleCollision
		: public ModuleInterface
	{
	public:
		ModuleCollision();
		~ModuleCollision();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		CollisionWorldPtr createCollisionWorld();
		void removeCollisionWorld( const CollisionWorldPtr & _collision );

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef FactoryPoolStore<CollisionWorld, 4> TFactoryCollisionWorld;
		TFactoryCollisionWorld m_factoryCollisionWorld;

		typedef stdex::vector<CollisionWorldPtr> TVectorCollisionWorlds;
		TVectorCollisionWorlds m_collisionWorlds;
	};
}