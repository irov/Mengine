#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PathfinderFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OzzFramework" )

    public:
        PathfinderFrameworkPlugin();
        ~PathfinderFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}