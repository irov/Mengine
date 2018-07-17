#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
	class AstralaxParticlePlugin2
		: public PluginBase
	{
		PLUGIN_DECLARE("AstralaxParticle")

	public:
		AstralaxParticlePlugin2();
        ~AstralaxParticlePlugin2() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _destroy() override;
	};
}