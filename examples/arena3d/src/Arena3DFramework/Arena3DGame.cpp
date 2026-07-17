#include "Arena3DGame.h"

#include "Arena3DSceneEventReceiver.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool Arena3DGame::_initializeGame()
    {
        Arena3DSceneEventReceiverPtr receiver = Helper::makeFactorableUnique<Arena3DSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );
        ScenePtr scene = Helper::makeScene( receiver, MENGINE_DOCUMENT_FACTORABLE );
        scene->setName( STRINGIZE_STRING_LOCAL( "Arena3DScene" ) );
        SCENE_SERVICE()->setCurrentScene( scene, false, false, nullptr );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DGame::_finalizeGame()
    {
        SCENE_SERVICE()->removeCurrentScene( true, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
