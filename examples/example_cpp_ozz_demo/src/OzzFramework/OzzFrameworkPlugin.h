#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class OzzFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OzzFramework" )

    public:
        OzzFrameworkPlugin();
        ~OzzFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}