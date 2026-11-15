#pragma once

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Win32OpButtonScriptEmbedding.h"
#endif

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class Win32OpButtonPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( Win32OpButtonScriptEmbedding )
    {
        PLUGIN_DECLARE( "Win32OpButton" );
        DECLARE_EMBEDDABLE();

    public:
        Win32OpButtonPlugin();
        ~Win32OpButtonPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
