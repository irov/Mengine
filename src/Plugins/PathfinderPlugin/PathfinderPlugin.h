#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "PathfinderScriptEmbedding.h"
#endif

namespace Mengine
{
    class PathfinderPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( PathfinderScriptEmbedding )
    {
        PLUGIN_DECLARE( "Pathfinder" );
        DECLARE_EMBEDDABLE();

    public:
        PathfinderPlugin();
        ~PathfinderPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}