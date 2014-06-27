#	pragma once

#	include "Interface/PluginInterface.h"

#	include "Interface/ParticleSystemInterface2.h"

namespace Menge
{
	class AstralaxParticlePlugin2
		: public PluginInterface
	{
	public:
		AstralaxParticlePlugin2();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ParticleSystemInterface2 * m_particleSystem2;
	};
}