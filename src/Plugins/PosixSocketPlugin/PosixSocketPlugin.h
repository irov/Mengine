#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PosixSocketPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PosixSocket" )

    public:
        PosixSocketPlugin();
        ~PosixSocketPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}