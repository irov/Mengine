#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImGUIPlugin
        : public PluginBase
        , public Observable
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
        void notifyCreateRenderWindow_();

    protected:
        uint32_t m_handlerId;
    };
}