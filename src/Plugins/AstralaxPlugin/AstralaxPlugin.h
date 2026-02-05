#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#include "AstralaxScriptEmbedding.h"
#endif

namespace Mengine
{
    class AstralaxPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AstralaxScriptEmbedding )
    {
        PLUGIN_DECLARE( "Astralax" );
        DECLARE_EMBEDDABLE();

    public:
        AstralaxPlugin();
        ~AstralaxPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroyPlugin() override;
    };
}