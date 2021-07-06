#include "PuzzleFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "PuzzleSceneEventReceiver.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PuzzleFramework::PuzzleFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PuzzleFramework::~PuzzleFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PuzzleFramework::_initializeFramework()
    {
        PuzzleSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<PuzzleSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "Puzzle" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PuzzleFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
