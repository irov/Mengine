#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class INIPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "INI" )

    public:
        INIPlugin();
        ~INIPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}