#pragma once

#include "JewelryGame.h"

#include "Interface/GameEventReceiverInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class JewelryGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "JewelryGame" )

    public:
        JewelryGamePlugin();
        ~JewelryGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        JewelryGamePtr m_game;
    };
}