#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PVRTCPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PVRTC" )

    public:
        PVRTCPlugin();
        ~PVRTCPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}