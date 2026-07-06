#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "ImGUIScriptEmbedding.h"
#endif

namespace Mengine
{
    class ImGUIFrameworkPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( ImGUIScriptEmbedding )
    {
        PLUGIN_DECLARE( "ImGUIFramework" );
        DECLARE_EMBEDDABLE();

    public:
        ImGUIFrameworkPlugin();
        ~ImGUIFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
