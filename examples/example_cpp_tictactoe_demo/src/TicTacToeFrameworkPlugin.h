#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TicTacToeFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TicTacToeFramework" )

    public:
        TicTacToeFrameworkPlugin();
        ~TicTacToeFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}