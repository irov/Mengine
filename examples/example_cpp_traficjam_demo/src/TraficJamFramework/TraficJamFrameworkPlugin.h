#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TraficJamFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TraficJamFramework" )

    public:
        TraficJamFrameworkPlugin();
        ~TraficJamFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}