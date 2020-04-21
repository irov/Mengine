#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AstralaxPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AstralaxParticle" )

    public:
        AstralaxPlugin();
        ~AstralaxPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}