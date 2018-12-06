#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class XmlToBinPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "XmlToBin" )

    public:
        XmlToBinPlugin();
        ~XmlToBinPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}