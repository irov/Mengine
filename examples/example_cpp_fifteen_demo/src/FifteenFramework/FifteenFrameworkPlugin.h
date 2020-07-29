#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FifteenFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FifteenFramework" )

    public:
        FifteenFrameworkPlugin();
        ~FifteenFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}