#pragma once

#include "Arena3DGame.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Arena3DGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Arena3DGame" )

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    private:
        Arena3DGamePtr m_game;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
