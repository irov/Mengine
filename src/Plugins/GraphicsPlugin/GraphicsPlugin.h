#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "GraphicsScriptEmbedding.h"
#endif

namespace Mengine
{
    class GraphicsPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( GraphicsScriptEmbedding )
    {
        PLUGIN_DECLARE( "Graphics" );
        DECLARE_EMBEDDABLE();

    public:
        GraphicsPlugin();
        ~GraphicsPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}