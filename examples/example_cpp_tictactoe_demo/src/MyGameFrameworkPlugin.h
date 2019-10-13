#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class MyGameFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MyGameFramework" )

    public:
        MyGameFrameworkPlugin();
        ~MyGameFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}