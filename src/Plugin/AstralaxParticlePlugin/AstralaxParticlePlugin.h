#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Visitor.h"

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
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;

    protected:
        VisitorPtr m_particleValidateVisitor;
    };
}