#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class PluginAmplifier
		: public PluginBase
	{
		PLUGIN_DECLARE( "Amplifier" )

	public:
		PluginAmplifier();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _destroy() override;
	};
}