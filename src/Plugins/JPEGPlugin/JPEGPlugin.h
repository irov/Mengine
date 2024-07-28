#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class JPEGPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "JPEG" )

    public:
        JPEGPlugin();
        ~JPEGPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}