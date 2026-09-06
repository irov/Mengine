#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MacOSFindPython3Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MacOSFindPython3" );

    public:
        MacOSFindPython3Plugin();
        ~MacOSFindPython3Plugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
