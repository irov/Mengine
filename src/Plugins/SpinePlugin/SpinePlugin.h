#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SpinePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Spine" )

    public:
        SpinePlugin();
        ~SpinePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}