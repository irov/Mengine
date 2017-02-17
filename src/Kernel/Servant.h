#	pragma once

#	include "Factory/FactorablePtr.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class Servant
		: public FactorablePtr
	{
	public:
		Servant();
		~Servant();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}