#include "FontEffectViewerExampleGame.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"

#include "FontEffectViewerExampleSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleGame::FontEffectViewerExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleGame::~FontEffectViewerExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleGame::_initializeGame()
    {
        FontEffectViewerExampleSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<FontEffectViewerExampleSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );
        scene->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerExampleScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleGame::_finalizeGame()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
