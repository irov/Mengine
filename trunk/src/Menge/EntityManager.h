#	pragma once

#   include "Interface/EntityInterface.h"

#	include "PrototypeManager.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class Entity;

	class EntityManager
		: public EntityServiceInterface
        , public PrototypeManager
	{
    public:
        EntityManager();
        ~EntityManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;
          
	public:
		Entity * createEntity( const ConstString & _prototype ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}