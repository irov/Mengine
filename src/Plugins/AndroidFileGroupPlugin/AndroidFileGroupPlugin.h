#	pragma once

#	include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidFileGroupPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidFileGroup" )

    public:
        AndroidFileGroupPlugin();
        ~AndroidFileGroupPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
