#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GOAPPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GOAP" );

    public:
        GOAPPlugin();
        ~GOAPPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}