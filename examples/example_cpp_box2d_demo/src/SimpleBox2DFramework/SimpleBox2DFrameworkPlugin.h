#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SimpleBox2DFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SimpleBox2DFramework" )

    public:
        SimpleBox2DFrameworkPlugin();
        ~SimpleBox2DFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}