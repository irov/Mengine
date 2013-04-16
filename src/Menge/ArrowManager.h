#	pragma once

#   include "Interface/ArrowInterface.h"

#	include "PrototypeManager.h"

namespace Menge
{
    class LogServiceInterface;

	class Arrow;

	class ArrowManager
        : public ArrowServiceInterface
		, public PrototypeManager
	{
    public:
        ArrowManager();
        ~ArrowManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		Arrow * createArrow( const ConstString & _prototype ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}