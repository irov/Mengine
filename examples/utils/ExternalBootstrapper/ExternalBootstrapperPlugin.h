#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ExternalBootstrapperPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ExternalBootstrapper" )

    public:
        ExternalBootstrapperPlugin();
        ~ExternalBootstrapperPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}