#	pragma once

#	include "Interface/PluginInterface.h"

#	include "Interface/ParticleSystemInterface.h"

namespace Menge
{
	class AstralaxParticlePlugin
		: public PluginInterface
	{
	public:
		AstralaxParticlePlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ParticleSystemInterface * m_particleSystem;
	};
}