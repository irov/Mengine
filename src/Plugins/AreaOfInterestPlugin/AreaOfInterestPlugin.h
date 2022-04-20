#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AreaOfInterestPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AreaOfInterest" )

    public:
        AreaOfInterestPlugin();
        ~AreaOfInterestPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}