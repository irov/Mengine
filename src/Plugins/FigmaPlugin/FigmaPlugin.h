#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "FigmaScriptEmbedding.h"
#endif

namespace Mengine
{
    class FigmaPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( FigmaScriptEmbedding )
    {
        PLUGIN_DECLARE( "Figma" );
        DECLARE_EMBEDDABLE();

    public:
        FigmaPlugin();
        ~FigmaPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
