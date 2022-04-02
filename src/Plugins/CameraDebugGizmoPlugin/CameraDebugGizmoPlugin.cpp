#include "CameraDebugGizmoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ModuleCameraDebugGizmo.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( CameraDebugGizmo, Mengine::CameraDebugGizmoPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CameraDebugGizmoPlugin::CameraDebugGizmoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CameraDebugGizmoPlugin::~CameraDebugGizmoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CameraDebugGizmoPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleCameraDebugGizmo" )
            , Helper::makeModuleFactory<ModuleCameraDebugGizmo>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CameraDebugGizmoPlugin::_finalizePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
