#pragma once

#include "VirtualAreaExampleGame.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaExampleGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "VirtualAreaExampleGame" )

    public:
        VirtualAreaExampleGamePlugin();
        ~VirtualAreaExampleGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        VirtualAreaExampleGamePtr m_game;
    };
    //////////////////////////////////////////////////////////////////////////
}
