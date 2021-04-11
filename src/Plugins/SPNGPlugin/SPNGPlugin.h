#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PNGPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PNG" )

    public:
        PNGPlugin();
        ~PNGPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}