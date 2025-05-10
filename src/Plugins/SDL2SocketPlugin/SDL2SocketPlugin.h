#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SDLSocketPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SDLSocket" )

    public:
        SDLSocketPlugin();
        ~SDLSocketPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}