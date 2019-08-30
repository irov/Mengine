#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class UIFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "UIFramework" )

    public:
        UIFrameworkPlugin();
        ~UIFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}