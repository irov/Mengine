#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "VideoScriptEmbedding.h"
#endif

namespace Mengine
{
    class VideoPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( VideoScriptEmbedding )
    {
        PLUGIN_DECLARE( "Video" );
        DECLARE_EMBEDDABLE();

    public:
        VideoPlugin();
        ~VideoPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

