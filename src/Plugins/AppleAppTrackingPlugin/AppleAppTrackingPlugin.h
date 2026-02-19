#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAppTrackingScriptEmbedding.h"
#endif

namespace Mengine
{
    class AppleAppTrackingPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleAppTrackingScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleAppTracking" );
        DECLARE_EMBEDDABLE();

    public:
        AppleAppTrackingPlugin();
        ~AppleAppTrackingPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
