#pragma once

#include "Kernel/PluginBase.h"

#include "FEInterface.h"

namespace Mengine
{
    class FEPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FE" );

    public:
        FEPlugin();
        ~FEPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}