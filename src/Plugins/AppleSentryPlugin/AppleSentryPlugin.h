#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleSentryPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleSentry" )

    public:
        AppleSentryPlugin();
        ~AppleSentryPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}