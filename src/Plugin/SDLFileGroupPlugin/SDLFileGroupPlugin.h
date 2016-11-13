#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
    class SDLFileGroupPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SDLFileGroup" )

    public:
        SDLFileGroupPlugin();

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
