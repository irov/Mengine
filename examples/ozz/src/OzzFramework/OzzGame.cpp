#include "OzzGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "OzzSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OzzGame::OzzGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzGame::~OzzGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzGame::_initializeGame()
    {
        OzzSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<OzzSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "OzzScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzGame::_finalizeGame()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
