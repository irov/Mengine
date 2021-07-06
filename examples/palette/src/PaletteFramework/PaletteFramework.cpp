#include "PaletteFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "PaletteSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PaletteFramework::PaletteFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PaletteFramework::~PaletteFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PaletteFramework::_initializeFramework()
    {
        PaletteSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<PaletteSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "MyGameScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PaletteFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
