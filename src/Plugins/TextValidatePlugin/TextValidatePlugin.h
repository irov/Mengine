#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TextValidatePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TextValidate" )

    public:
        TextValidatePlugin();
        ~TextValidatePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyBootstrapperInitializeGame_();
    };
}