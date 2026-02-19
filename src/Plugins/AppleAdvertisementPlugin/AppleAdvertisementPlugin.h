#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAdvertisementScriptEmbedding.h"
#endif

namespace Mengine
{    
    class AppleAdvertisementPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleAdvertisementScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleAdvertisement" );
        DECLARE_EMBEDDABLE();

    public:
        AppleAdvertisementPlugin();
        ~AppleAdvertisementPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override; 
    };
}
