#pragma once

#include "FifteenGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class FifteenGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FifteenGame" )

    public:
        FifteenGamePlugin();
        ~FifteenGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        FifteenGamePtr m_game;
    };
}