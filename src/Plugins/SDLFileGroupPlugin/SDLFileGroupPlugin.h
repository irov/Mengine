#	pragma once

#	include "Kernel/PluginBase.h"

namespace Mengine
{
    class SDLFileGroupPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SDLFileGroup" )

    public:
        SDLFileGroupPlugin();
        ~SDLFileGroupPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
