#pragma once

#include "Crimsonland3DGame.h"

#include "Interface/GameEventReceiverInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Crimsonland3DGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Crimsonland3DGame" )

    public:
        Crimsonland3DGamePlugin();
        ~Crimsonland3DGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        Crimsonland3DGamePtr m_game;
    };
}
