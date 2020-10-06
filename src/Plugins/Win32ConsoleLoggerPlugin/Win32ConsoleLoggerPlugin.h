#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ConsoleLoggerPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32ConsoleLogger" )

    public:
        Win32ConsoleLoggerPlugin();
        ~Win32ConsoleLoggerPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        LoggerInterfacePtr m_loggerConsole;
    };
}