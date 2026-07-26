#include "iOSFirebaseRemoteConfigScriptEmbedding.h"

#include "iOSFirebaseRemoteConfigInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#import "iOSFirebaseRemoteConfigPlugin.h"

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
        static bool iOSFirebaseRemoteConfigService_hasRemoteConfig( NSString * _key )
        {
            if( [[iOSFirebaseRemoteConfigPlugin sharedInstance] hasRemoteConfig:_key] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSDictionary * iOSFirebaseRemoteConfigService_getRemoteConfigValue( NSString * _key )
        {
            NSDictionary * params = [[iOSFirebaseRemoteConfigPlugin sharedInstance] getRemoteConfigValue:_key];

            return params;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFirebaseRemoteConfigScriptEmbedding::iOSFirebaseRemoteConfigScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFirebaseRemoteConfigScriptEmbedding::~iOSFirebaseRemoteConfigScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFirebaseRemoteConfigScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "iOSFirebaseRemoteConfigHasValue", &Detail::iOSFirebaseRemoteConfigService_hasRemoteConfig );
        pybind::def_function( _kernel, "iOSFirebaseRemoteConfigGetValue", &Detail::iOSFirebaseRemoteConfigService_getRemoteConfigValue );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFirebaseRemoteConfigScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSFirebaseRemoteConfigHasValue", nullptr );
        _kernel->remove_from_module( "iOSFirebaseRemoteConfigGetValue", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
