#include "TraficJamFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "TraficJamSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TraficJamFramework::TraficJamFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TraficJamFramework::~TraficJamFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamFramework::_initializeFramework()
    {
        TraficJamSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<TraficJamSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "TraficJam" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamFramework::_finalizeFramework()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
}
