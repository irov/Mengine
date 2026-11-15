#pragma once

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Win32NativePythonScriptEmbedding.h"
#endif

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32NativePythonPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( Win32NativePythonScriptEmbedding )
    {
        PLUGIN_DECLARE( "Win32NativePython" );
        DECLARE_EMBEDDABLE();

    public:
        Win32NativePythonPlugin();
        ~Win32NativePythonPlugin() override;
    };
}
