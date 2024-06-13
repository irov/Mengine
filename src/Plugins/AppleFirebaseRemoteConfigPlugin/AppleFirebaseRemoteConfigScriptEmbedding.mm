#include "AppleFirebaseRemoteConfigScriptEmbedding.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigScriptEmbedding::AppleFirebaseRemoteConfigScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigScriptEmbedding::~AppleFirebaseRemoteConfigScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleFirebaseRemoteConfig", true );
        
        AppleFirebaseRemoteConfigServiceInterface * service = APPLE_FIREBASE_REMOTECONFIG_SERVICE();

        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigGetValueBoolean", service, &AppleFirebaseRemoteConfigScriptEmbedding::getValueBoolean );
        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigGetValueInteger", service, &AppleFirebaseRemoteConfigScriptEmbedding::getValueInteger );
        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigGetValueDouble", service, &AppleFirebaseRemoteConfigScriptEmbedding::getValueDouble );
        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigGetValueConstString", service, &AppleFirebaseRemoteConfigScriptEmbedding::getValueConstString );
        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigGetValueJSON", service, &AppleFirebaseRemoteConfigScriptEmbedding::getValueJSON );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValueBoolean", nullptr );
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValueInteger", nullptr );
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValueDouble", nullptr );
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValueConstString", nullptr );
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValueJSON", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

