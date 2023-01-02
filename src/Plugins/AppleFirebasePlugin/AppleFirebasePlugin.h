#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebasePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebase" )

    public:
        AppleFirebasePlugin();
        ~AppleFirebasePlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
