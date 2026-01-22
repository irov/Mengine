#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class Win32SentryPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Sentry" );

    public:
        Win32SentryPlugin();
        ~Win32SentryPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}