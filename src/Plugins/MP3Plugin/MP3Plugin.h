#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class MP3Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MP3" );

    public:
        MP3Plugin();
        ~MP3Plugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
