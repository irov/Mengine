#pragma once

#include "CastleRoyalGame.h"

#include "Interface/GameEventReceiverInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class CastleRoyalGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "CastleRoyalGame" )

    public:
        CastleRoyalGamePlugin();
        ~CastleRoyalGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        CastleRoyalGamePtr m_game;
    };
}