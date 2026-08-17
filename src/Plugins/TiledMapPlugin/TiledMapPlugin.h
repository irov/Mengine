#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "TiledMapScriptEmbedding.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TiledMapPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( TiledMapScriptEmbedding )
    {
        PLUGIN_DECLARE( "TiledMap" );
        DECLARE_EMBEDDABLE();

    public:
        TiledMapPlugin();
        ~TiledMapPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
