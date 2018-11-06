#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class XmlCodecPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "XmlCodec" )

    public:
        XmlCodecPlugin();

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}