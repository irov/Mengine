#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAdMobScriptEmbedding.h"
#endif

namespace Mengine
{    
    class AppleAdMobPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleAdMobScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleAdMob" );
        DECLARE_EMBEDDABLE();

    public:
        AppleAdMobPlugin();
        ~AppleAdMobPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override; 
    };
}

