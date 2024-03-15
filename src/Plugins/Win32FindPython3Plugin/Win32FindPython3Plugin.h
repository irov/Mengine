#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32FindPython3Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32FindPython3" )

    public:
        Win32FindPython3Plugin();
        ~Win32FindPython3Plugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}