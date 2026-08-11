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
                copy_cb.call_args( granted == YES, copy_args );
            }];
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSUserNotificationCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSUserNotificationCenterRequestAuthorization", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
