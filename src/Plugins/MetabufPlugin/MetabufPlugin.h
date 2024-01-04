#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class MetabufPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Metabuf" );

    public:
        MetabufPlugin();
        ~MetabufPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}