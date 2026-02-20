#pragma once

#include "Kernel/PluginBase.h"

#include "AndroidNativePythonScriptEmbedding.h"

namespace Mengine
{
    class AndroidNativePythonPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE(AndroidNativePythonScriptEmbedding)
    {
        PLUGIN_DECLARE( "AndroidNativePython" )
        DECLARE_EMBEDDABLE();

    public:
        AndroidNativePythonPlugin();
        ~AndroidNativePythonPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
