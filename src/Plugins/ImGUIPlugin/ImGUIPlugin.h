#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ImGUIPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ImGUI" )

    public:
        ImGUIPlugin();
        ~ImGUIPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;

    protected:
        uint32_t m_handlerId;
    };
}