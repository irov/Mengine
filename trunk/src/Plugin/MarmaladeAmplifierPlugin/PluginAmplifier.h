#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/AmplifierInterface.h"

namespace Menge
{
	class PluginAmplifier
		: public PluginInterface
	{
	public:
		PluginAmplifier();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		AmplifierInterface * m_amplifier;
	};
}