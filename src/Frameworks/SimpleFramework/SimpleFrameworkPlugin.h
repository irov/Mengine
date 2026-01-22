#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SimpleFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SimpleFramework" );

    public:
        SimpleFrameworkPlugin();
        ~SimpleFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}