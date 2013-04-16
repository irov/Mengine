#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DecoderFactoryInterface;

	class XlsExportPlugin
		: public PluginInterface
	{
	public:
		XlsExportPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
		bool proccess( const String & _projectName );

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}