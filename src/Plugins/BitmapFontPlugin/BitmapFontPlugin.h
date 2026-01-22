#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class BitmapFontPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "BitmapFont" );

    public:
        BitmapFontPlugin();
        ~BitmapFontPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}