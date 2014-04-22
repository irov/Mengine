#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class Win32FileGroupPlugin
		: public PluginInterface
	{
	public:
		Win32FileGroupPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}