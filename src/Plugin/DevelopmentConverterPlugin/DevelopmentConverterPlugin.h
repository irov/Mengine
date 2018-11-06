#pragma once

#include "Interface/ConverterInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class DevelopmentConverterPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DevelopmentConverter" )

    public:
        DevelopmentConverterPlugin();
        ~DevelopmentConverterPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}