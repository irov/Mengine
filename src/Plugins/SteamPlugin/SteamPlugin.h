#pragma once

#include "Kernel/PluginBase.h"

#include "steam/steam_api.h"

namespace Mengine
{
    class SteamPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Steam" )

    public:
        SteamPlugin();
        ~SteamPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}