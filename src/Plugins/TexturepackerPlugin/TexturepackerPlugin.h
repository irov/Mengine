#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "TexturepackerScriptEmbedding.h"
#endif

namespace Mengine
{
    class TexturepackerPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( TexturepackerScriptEmbedding )
    {
        PLUGIN_DECLARE( "Texturepacker" );
        DECLARE_EMBEDDABLE();

    public:
        TexturepackerPlugin();
        ~TexturepackerPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

