#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidSoundPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidSound" );

    public:
        AndroidSoundPlugin();
        ~AndroidSoundPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
