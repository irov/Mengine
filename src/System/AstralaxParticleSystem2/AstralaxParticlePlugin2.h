#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class AstralaxParticlePlugin2
		: public PluginInterface
	{
	public:
		AstralaxParticlePlugin2();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}