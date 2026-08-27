#pragma once

#include "Kernel/PluginBase.h"

#include "TiledMapExampleGame.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TiledMapExampleGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TiledMapExampleGame" )

    public:
        TiledMapExampleGamePlugin();
        ~TiledMapExampleGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        TiledMapExampleGamePtr m_game;
    };
    //////////////////////////////////////////////////////////////////////////
}
