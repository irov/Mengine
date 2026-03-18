#pragma once

#include "PathfinderGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class PathfinderGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PathfinderGame" )

    public:
        PathfinderGamePlugin();
        ~PathfinderGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        PathfinderGamePtr m_game;
    };
}