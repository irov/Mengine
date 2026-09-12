#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "FontEffectScriptEmbedding.h"
#endif

namespace Mengine
{
    class FontEffectPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( FontEffectScriptEmbedding )
    {
        PLUGIN_DECLARE( "FontEffect" );
        DECLARE_EMBEDDABLE();

    public:
        FontEffectPlugin();
        ~FontEffectPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
