#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleFirebaseRemoteConfigScriptEmbedding.h"
#endif

namespace Mengine
{
    class AppleFirebaseRemoteConfigPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleFirebaseRemoteConfigScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleFirebaseRemoteConfig" );
        DECLARE_EMBEDDABLE();

    public:
        AppleFirebaseRemoteConfigPlugin();
        ~AppleFirebaseRemoteConfigPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
