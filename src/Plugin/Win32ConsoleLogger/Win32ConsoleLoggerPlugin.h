#pragma once

#include "Interface/LoggerInterface.h"

#include "Core/PluginBase.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class Win32ConsoleLoggerPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Win32ConsoleLogger" )

	public:
        Win32ConsoleLoggerPlugin();

    protected:
        bool _avaliable() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

    protected:
        LoggerInterfacePtr m_loggerConsole;
	};
}