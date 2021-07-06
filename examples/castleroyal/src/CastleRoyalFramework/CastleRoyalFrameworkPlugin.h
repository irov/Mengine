#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class CastleRoyalFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SimpleBox2DFramework" )

    public:
        CastleRoyalFrameworkPlugin();
        ~CastleRoyalFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}