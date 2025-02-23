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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PyObject * AppleFirebaseRemoteConfigService_GetValue( pybind::kernel_interface * _kernel, const ConstString & _key )
        {
            Params params;
            if( APPLE_FIREBASE_REMOTECONFIG_SERVICE()
               ->getRemoteConfigValue( _key, &params ) == false )
            {
                return _kernel->ret_none();
            }
            
            PyObject * py_params = pybind::ptr( _kernel, params );
            
            return py_params;
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleFirebaseRemoteConfig"), true );
        
        AppleFirebaseRemoteConfigServiceInterface * service = APPLE_FIREBASE_REMOTECONFIG_SERVICE();

        pybind::def_functor( _kernel, "appleFirebaseRemoteConfigHasValue", service, &AppleFirebaseRemoteConfigServiceInterface::hasRemoteConfig );
        pybind::def_function_kernel( _kernel, "appleFirebaseRemoteConfigGetValue", &Detail::AppleFirebaseRemoteConfigService_GetValue );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleFirebaseRemoteConfigHasValue", nullptr );
        _kernel->remove_from_module( "appleFirebaseRemoteConfigGetValue", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

