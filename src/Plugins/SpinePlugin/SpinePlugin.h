#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "SpineScriptEmbedding.h"
#endif

namespace Mengine
{
    class SpinePlugin
        : public PluginBase
        , EXTEND_EMBEDDABLE( SpineScriptEmbedding )
    {
        PLUGIN_DECLARE( "Spine" );
        DECLARE_EMBEDDABLE();

    public:
        SpinePlugin();
        ~SpinePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}