#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32FileGroupPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32FileGroup" )

    public:
        Win32FileGroupPlugin();
        ~Win32FileGroupPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}