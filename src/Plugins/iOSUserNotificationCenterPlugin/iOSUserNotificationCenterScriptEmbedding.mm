#include "iOSUserNotificationCenterScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "iOSUserNotificationCenterPlugin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSUserNotificationCenter_requestAuthorization( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            [[iOSUserNotificationCenterPlugin sharedInstance] requestAuthorization:^(BOOL granted) {
                copy_cb.call_args( granted, copy_args );
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSUserNotificationCenter_setNotificationDelay( NSString * _identifier, double _delay )
        {
            [[iOSUserNotificationCenterPlugin sharedInstance] setNotificationDelay:_identifier delay:_delay];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSUserNotificationCenterScriptEmbedding::iOSUserNotificationCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSUserNotificationCenterScriptEmbedding::~iOSUserNotificationCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSUserNotificationCenterScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "iOSUserNotificationCenterRequestAuthorization", &Detail::iOSUserNotificationCenter_requestAuthorization );
        pybind::def_function( _kernel, "iOSUserNotificationCenterSetNotificationDelay", &Detail::iOSUserNotificationCenter_setNotificationDelay );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSUserNotificationCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSUserNotificationCenterRequestAuthorization", nullptr );
        _kernel->remove_from_module( "iOSUserNotificationCenterSetNotificationDelay", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
