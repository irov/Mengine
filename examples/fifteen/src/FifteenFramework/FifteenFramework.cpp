#include "FifteenFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "FifteenSceneEventReceiver.h"

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
    FifteenFramework::FifteenFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FifteenFramework::~FifteenFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenFramework::_initializeFramework()
    {
        FifteenSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<FifteenSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "Fifteen" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
