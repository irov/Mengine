#include "CameraDebugGizmoPlugin.h"

#include "ModuleCameraDebugGizmo.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

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
    bool CameraDebugGizmoPlugin::_availablePlugin() const
    {
        bool CameraDebugGizmoPlugin_Available = CONFIG_VALUE( "CameraDebugGizmoPlugin", "Available", true );

        if( CameraDebugGizmoPlugin_Available == false )
        {
            return false;
        }

        return true;
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
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
