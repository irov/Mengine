#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32SocketPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32Socket" );

    public:
        Win32SocketPlugin();
        ~Win32SocketPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}