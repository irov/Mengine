#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class Win32SocketPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE("Win32Socket")

	public:
		Win32SocketPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _destroy() override;
	};
}