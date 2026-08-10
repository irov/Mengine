#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Box2DScriptEmbedding.h"
#endif

namespace Mengine
{    
    class Box2DPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( Box2DScriptEmbedding )
    {
        PLUGIN_DECLARE( "Box2D" )
        DECLARE_EMBEDDABLE();

    public:
        Box2DPlugin();
        ~Box2DPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
