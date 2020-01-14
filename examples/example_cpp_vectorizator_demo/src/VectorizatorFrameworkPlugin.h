#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class VectorizatorFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "VectorizatorFramework" )

    public:
        VectorizatorFrameworkPlugin();
        ~VectorizatorFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}