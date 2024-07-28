#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ETC1Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ETC1" )

    public:
        ETC1Plugin();
        ~ETC1Plugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}