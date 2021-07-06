#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Box2DFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Box2DFramework" )

    public:
        Box2DFrameworkPlugin();
        ~Box2DFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}