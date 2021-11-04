#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GameAnalyticsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GameAnalytics" )

    public:
        GameAnalyticsPlugin();
        ~GameAnalyticsPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;

    protected:
        void notifyAccountsLoad_();

    protected:
        Char m_gameKey[256];
        Char m_gameSecret[256];
    };
}