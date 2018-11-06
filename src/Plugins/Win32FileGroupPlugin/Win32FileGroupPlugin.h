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

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}