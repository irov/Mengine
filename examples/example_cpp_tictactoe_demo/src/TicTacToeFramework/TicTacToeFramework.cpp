#include "TicTacToeFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "TicTacToeSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TicTacToeFramework::TicTacToeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TicTacToeFramework::~TicTacToeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TicTacToeFramework::_initializeFramework()
    {
        TicTacToeSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<TicTacToeSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "TicTacToe" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TicTacToeFramework::_finalizeFramework()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
}
