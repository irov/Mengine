#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleGameCenterScriptEmbedding.h"
#endif

namespace Mengine
{
    class AppleGameCenterPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleGameCenterScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleGameCenter" );
        DECLARE_EMBEDDABLE();

    public:
        AppleGameCenterPlugin();
        ~AppleGameCenterPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
