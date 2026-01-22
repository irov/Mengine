#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class DevelopmentConverterPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DevelopmentConverter" );

    public:
        DevelopmentConverterPlugin();
        ~DevelopmentConverterPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}