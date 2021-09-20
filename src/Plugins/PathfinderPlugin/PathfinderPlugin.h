#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PathfinderPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Pathfinder" )

    public:
        PathfinderPlugin();
        ~PathfinderPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}