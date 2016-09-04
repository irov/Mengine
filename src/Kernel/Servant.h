#	pragma once

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class Servant
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