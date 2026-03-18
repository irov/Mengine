#pragma once

#include "OzzGame.h"

#include "Interface/GameEventReceiverInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class OzzGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OzzGame" )

    public:
        OzzGamePlugin();
        ~OzzGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        OzzGamePtr m_game;
    };
}