#pragma once

#include "Interface/DecoderFactoryInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AV1Plugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "AV1" )

    public:
        AV1Plugin();
        ~AV1Plugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

