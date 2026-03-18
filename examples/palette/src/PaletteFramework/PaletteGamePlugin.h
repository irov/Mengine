#pragma once

#include "PaletteGame.h"

#include "Interface/GameEventReceiverInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PaletteGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PaletteGame" )

    public:
        PaletteGamePlugin();
        ~PaletteGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        PaletteGamePtr m_game;
    };
}