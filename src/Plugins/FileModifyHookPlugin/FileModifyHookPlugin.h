#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class FileModifyHookPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FileModifyHook" )

    public:
        FileModifyHookPlugin();
        ~FileModifyHookPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}