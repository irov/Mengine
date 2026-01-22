#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class XmlToBinPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "XmlToBin" );

    public:
        XmlToBinPlugin();
        ~XmlToBinPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}