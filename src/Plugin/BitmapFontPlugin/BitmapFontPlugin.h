#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "BitmapFont" )

    public:
        BitmapFontPlugin();

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}