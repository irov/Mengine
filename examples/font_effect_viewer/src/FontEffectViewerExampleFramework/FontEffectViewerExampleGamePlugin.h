#pragma once

#include "FontEffectViewerExampleGame.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectViewerExampleGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FontEffectViewerExampleGame" )

    public:
        FontEffectViewerExampleGamePlugin();
        ~FontEffectViewerExampleGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        FontEffectViewerExampleGamePtr m_game;
    };
    //////////////////////////////////////////////////////////////////////////
}
