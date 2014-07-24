#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class LZ4Plugin
		: public PluginInterface
	{
	public:
		LZ4Plugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}