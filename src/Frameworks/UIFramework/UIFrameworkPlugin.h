#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "UIFrameworkScriptEmbedding.h"
#endif

namespace Mengine
{
    class UIFrameworkPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( UIFrameworkScriptEmbedding )
    {
        PLUGIN_DECLARE( "UIFramework" )
        DECLARE_EMBEDDABLE();

    public:
        UIFrameworkPlugin();
        ~UIFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
