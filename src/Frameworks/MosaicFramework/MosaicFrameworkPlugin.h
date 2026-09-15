#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "MosaicScriptEmbedding.h"
#endif

namespace Mengine
{
    class MosaicFrameworkPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( MosaicScriptEmbedding )
    {
        PLUGIN_DECLARE( "MosaicFramework" );
        DECLARE_EMBEDDABLE();

    public:
        MosaicFrameworkPlugin();
        ~MosaicFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
