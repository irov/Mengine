#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class JewelryFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "JewelryFramework" )

    public:
        JewelryFrameworkPlugin();
        ~JewelryFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}