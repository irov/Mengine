#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class WAVPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "WAV" )

    public:
        WAVPlugin();
        ~WAVPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}