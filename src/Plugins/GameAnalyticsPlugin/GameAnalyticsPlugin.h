#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/StaticString.h"

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
        bool loadUserId_();
        bool saveUserId_();
        const Char * getUserId_();

    protected:
        StaticString<21> m_userId;
        StaticString<256> m_gameKey;
        StaticString<256> m_gameSecret;
    };
}