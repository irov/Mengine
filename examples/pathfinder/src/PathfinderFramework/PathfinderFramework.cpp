#include "PathfinderFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "PathfinderEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderFramework::PathfinderFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderFramework::~PathfinderFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderFramework::_initializeFramework()
    {
        PathfinderEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<PathfinderEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "PathfinderScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
