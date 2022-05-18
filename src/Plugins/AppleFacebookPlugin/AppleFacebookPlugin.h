#pragma once

#include "Interface/PlatformInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleFacebookPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFacebook" )

    public:
        AppleFacebookPlugin();
        ~AppleFacebookPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;     
    };
}

