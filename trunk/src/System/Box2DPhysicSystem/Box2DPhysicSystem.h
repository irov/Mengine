#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include "Box2DPhysicWorld.h"

#	include "Box2D/Box2D.h"

namespace Menge
{
    class Box2DPhysicSystem
        : public PhysicSystemInterface		
    {
    public:
        Box2DPhysicSystem();
        ~Box2DPhysicSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
        void finalize() override;

	public:
		PhysicWorldInterface * createWorld( const ConstString & _name, float _gravityX, float _gravityY ) override;
		void destroyWorld( const ConstString & _name ) override;

    public:
        void update( float _timing, int _velocityIterations, int _positionIterations ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		struct WorldDesc
		{
			ConstString name;
			Box2DPhysicWorld * world;
		};

		typedef std::vector<WorldDesc> TVectorWorlds;
		TVectorWorlds m_worlds;
    };
}