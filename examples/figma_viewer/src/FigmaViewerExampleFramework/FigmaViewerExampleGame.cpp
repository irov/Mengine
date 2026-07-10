#include "FigmaViewerExampleGame.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"

#include "FigmaViewerExampleSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FigmaViewerExampleGame::FigmaViewerExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaViewerExampleGame::~FigmaViewerExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleGame::_initializeGame()
    {
        FigmaViewerExampleSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<FigmaViewerExampleSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );
        scene->setName( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
