#pragma once

#include "FigmaViewerExampleGame.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FigmaViewerExampleGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FigmaViewerExampleGame" )

    public:
        FigmaViewerExampleGamePlugin();
        ~FigmaViewerExampleGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        FigmaViewerExampleGamePtr m_game;
    };
    //////////////////////////////////////////////////////////////////////////
}
