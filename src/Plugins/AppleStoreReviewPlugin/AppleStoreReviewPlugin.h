#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class AppleStoreReviewPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleStoreReview" )

    public:
        AppleStoreReviewPlugin();
        ~AppleStoreReviewPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

