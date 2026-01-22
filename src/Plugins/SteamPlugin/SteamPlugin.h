#pragma once

#include "Kernel/PluginBase.h"

#include "steam/steam_api.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#include "SteamScriptEmbedding.h"
#endif

namespace Mengine
{
    class SteamPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( SteamScriptEmbedding )
    {
        PLUGIN_DECLARE( "Steam" );
        DECLARE_VISITABLE( PluginInterface );
        DECLARE_EMBEDDABLE();

    public:
        SteamPlugin();
        ~SteamPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}