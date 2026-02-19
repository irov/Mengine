#pragma once

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleStoreReviewScriptEmbedding.h"
#endif

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleStoreReviewPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( AppleStoreReviewScriptEmbedding )
    {
        PLUGIN_DECLARE( "AppleStoreReview" );
        DECLARE_EMBEDDABLE();

    public:
        AppleStoreReviewPlugin();
        ~AppleStoreReviewPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

