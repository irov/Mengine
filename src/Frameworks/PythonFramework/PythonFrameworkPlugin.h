#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PythonFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PythonFramework" )

    public:
        PythonFrameworkPlugin();
        ~PythonFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}