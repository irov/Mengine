#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AstralaxParticlePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AstralaxParticle" )

    public:
        AstralaxParticlePlugin();
        ~AstralaxParticlePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}