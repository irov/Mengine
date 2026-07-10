#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FigmaViewerExampleGame
        : public GameBase
    {
        DECLARE_FACTORABLE( FigmaViewerExampleGame );

    public:
        FigmaViewerExampleGame();
        ~FigmaViewerExampleGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FigmaViewerExampleGame> FigmaViewerExampleGamePtr;
    //////////////////////////////////////////////////////////////////////////
}
