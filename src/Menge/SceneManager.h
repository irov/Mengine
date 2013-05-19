#	pragma once

#   include "Interface/SceneInterface.h"

#	include "PrototypeManager.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class Scene;
	
	class SceneManager
        : public SceneServiceInterface
		, public PrototypeManager
	{
    public:
        SceneManager();
        ~SceneManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:		
		Scene * createScene( const ConstString & _name ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}