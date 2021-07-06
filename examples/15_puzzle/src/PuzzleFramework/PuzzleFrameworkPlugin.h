#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PuzzleFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PuzzleFramework" )

    public:
        PuzzleFrameworkPlugin();
        ~PuzzleFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}