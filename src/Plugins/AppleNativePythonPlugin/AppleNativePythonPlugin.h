#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleNativePythonScriptEmbedding.h"
#endif

namespace Mengine
{
    class AppleNativePythonPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleNativePythonScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleNativePython" );
        DECLARE_EMBEDDABLE();

    public:
        AppleNativePythonPlugin();
        ~AppleNativePythonPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
