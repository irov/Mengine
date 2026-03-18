#pragma once

#include "Box2DGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class Box2DGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Box2DGame" )

    public:
        Box2DGamePlugin();
        ~Box2DGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        Box2DGamePtr m_game;
    };
}