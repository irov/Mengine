#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"

#	include <vector>

namespace Menge
{
	class ZipPlugin
		: public PluginInterface
	{
	public:
		ZipPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}