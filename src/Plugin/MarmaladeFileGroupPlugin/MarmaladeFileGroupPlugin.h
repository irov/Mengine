#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class MarmaladeFileGroupPlugin
		: public PluginInterface
	{
	public:
		MarmaladeFileGroupPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}