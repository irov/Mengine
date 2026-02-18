#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AreaOfInterestScriptEmbedding.h"
#endif

namespace Mengine
{
    class AreaOfInterestPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AreaOfInterestScriptEmbedding )
    {
        PLUGIN_DECLARE( "AreaOfInterest" );
        DECLARE_EMBEDDABLE();

    public:
        AreaOfInterestPlugin();
        ~AreaOfInterestPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}