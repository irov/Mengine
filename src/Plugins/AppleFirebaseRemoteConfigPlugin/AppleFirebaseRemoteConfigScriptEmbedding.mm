#include "AppleFirebaseRemoteConfigScriptEmbedding.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

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
        static bool AppleFirebaseRemoteConfigService_hasRemoteConfig( NSString * _key )
        {
            if( [[AppleFirebaseRemoteConfigApplicationDelegate sharedInstance] hasRemoteConfig:_key] == NO )
            {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSDictionary * AppleFirebaseRemoteConfigService_getRemoteConfigValue( NSString * _key )
        {
            NSDictionary * params = [[AppleFirebaseRemoteConfigApplicationDelegate sharedInstance] getRemoteConfigValue:_key];
            
            return params;
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
        pybind::def_function( _kernel, "appleFirebaseRemoteConfigHasValue", &Detail::AppleFirebaseRemoteConfigService_hasRemoteConfig );
        pybind::def_function( _kernel, "appleFirebaseRemoteConfigGetValue", &Detail::AppleFirebaseRemoteConfigService_getRemoteConfigValue );
        
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

