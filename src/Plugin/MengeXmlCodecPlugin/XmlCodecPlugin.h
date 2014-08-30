#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DecoderFactoryInterface;

	class XmlCodecPlugin
		: public PluginInterface
	{
	public:
		XmlCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}