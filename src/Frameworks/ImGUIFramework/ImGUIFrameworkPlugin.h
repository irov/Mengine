#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ImGUIFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ImGUIFramework" );

    public:
        ImGUIFrameworkPlugin();
        ~ImGUIFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
