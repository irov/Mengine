#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32SocketPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32Socket" )

    public:
        Win32SocketPlugin();
        ~Win32SocketPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}