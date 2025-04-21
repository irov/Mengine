#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleDevToDevPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleDevToDev" )

    public:
        AppleDevToDevPlugin();
        ~AppleDevToDevPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        
    protected:
        AnalyticsEventProviderInterfacePtr m_provider;
    };
}
