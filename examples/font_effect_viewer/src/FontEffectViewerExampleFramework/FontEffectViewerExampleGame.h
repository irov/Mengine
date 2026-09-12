#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectViewerExampleGame
        : public GameBase
    {
        DECLARE_FACTORABLE( FontEffectViewerExampleGame );

    public:
        FontEffectViewerExampleGame();
        ~FontEffectViewerExampleGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectViewerExampleGame> FontEffectViewerExampleGamePtr;
    //////////////////////////////////////////////////////////////////////////
}
