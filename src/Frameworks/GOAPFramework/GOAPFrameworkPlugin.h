#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GOAPFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GOAPFramework" )

    public:
        GOAPFrameworkPlugin();
        ~GOAPFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}